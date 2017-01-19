#include <BWEM/bwem.h>
#include <BuildingPlaceabilityHelper.hpp>
#include <Unit.hpp>

BuildingPlaceabilityHelper::BuildingPlaceabilityHelper(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

bool BuildingPlaceabilityHelper::canBuild(BWAPI::UnitType unit, BWAPI::TilePosition position, Unit* builder)
{
  BWAPI::TilePosition tileSize = unit.tileSize();
  BWAPI::Position rightBottom = BWAPI::Position(position + tileSize);
  BWAPI::Unitset result = BWAPI::Broodwar->getUnitsInRectangle(BWAPI::Position(position), rightBottom);
  for (BWAPI::Unit unit: result)
    if (builder == nullptr || unit != builder->getBWAPIUnit())
      return false;
  bool needsCreep = (unit != BWAPI::UnitTypes::Zerg_Hatchery);
  for (int32_t x = position.x; x < position.x + tileSize.x; ++x)
    for (int32_t y = position.y; y < position.y + tileSize.y; ++y)
      if (!BWEM::Map::Instance().GetTile(BWAPI::TilePosition(x,  y)).Buildable() ||
          needsCreep && !BWAPI::Broodwar->hasCreep(BWAPI::TilePosition(x, y)) ||
          this->tilesPlannedToTake.count(BWAPI::TilePosition(x, y)))
        return false;
  return true;
}

void BuildingPlaceabilityHelper::registerBuild(BWAPI::UnitType unit, BWAPI::TilePosition position)
{
  BWAPI::TilePosition tileSize = unit.tileSize();
  for (int32_t x = position.x; x < position.x + tileSize.x; ++x)
    for (int32_t y = position.y; y < position.y + tileSize.y; ++y)
      tilesPlannedToTake.insert(BWAPI::TilePosition(x, y));
}

void BuildingPlaceabilityHelper::unRegisterBuild(BWAPI::UnitType unit, BWAPI::TilePosition position)
{
    BWAPI::TilePosition tileSize = unit.tileSize();
  for (int32_t x = position.x; x < position.x + tileSize.x; ++x)
    for (int32_t y = position.y; y < position.y + tileSize.y; ++y)
      tilesPlannedToTake.erase(BWAPI::TilePosition(x, y));
}

void BuildingPlaceabilityHelper::onFrame()
{
  for (const BWAPI::TilePosition& position: this->tilesPlannedToTake)
  {
    BWAPI::Position leftTop;
    leftTop = (BWAPI::Position(position));
    BWAPI::Position rightBottom(BWAPI::TilePosition(position.x + 1, position.y + 1));
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(leftTop.x + 5, leftTop.y + 5),
                                BWAPI::Position(rightBottom.x - 5, rightBottom.y - 5),
                                BWAPI::Colors::Blue);
  }
}
