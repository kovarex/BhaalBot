#include <Units.hpp>
#include <Unit.hpp>
#include <Log.hpp>
#include <Player.hpp>

Units::Units(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

Units::~Units()
{
  for (Unit* unit: this->unitSet)
    delete unit;
}

void Units::onFrame()
{
  for (Unit* unit: this->unitSet)
    unit->updatePosition();
}

void Units::onEnd(bool isWinner)
{
  for (Unit* unit: this->unitSet)
    delete unit;
  this->unitSet.clear();
}

Unit* Units::onUnitComplete(BWAPI::Unit unit)
{
  auto position = this->units.find(unit);
  if (position != this->units.end())
  {
    // I assume that our unit just morphed into different unit here
    if (position->second->player && position->second->player->bwapiPlayer == unit->getPlayer() ||
        position->second->lastSeenUnitType == BWAPI::UnitTypes::Resource_Vespene_Geyser)
      return position->second;
    delete this->units[unit];
    this->units.erase(unit);
  }
#ifdef DEBUG
  for (auto& item: this->units)
    if (item.second->getID() == unit->getID())
    {
      LOG_AND_ABORT("Adding unit %s %d that is already there as different instance.", unit->getType().getName().c_str(), unit->getID());
    }
#endif
  Unit* ourUnit = new Unit(unit);
  this->units[unit] = ourUnit;
  this->unitSet.insert(ourUnit);
  return ourUnit;
}

void Units::onUnitDestroy(BWAPI::Unit unit)
{
  auto position = this->units.find(unit);
  if (position == this->units.end())
    return;
  this->unitSet.erase(position->second); // TODO should not need 2x find
  delete position->second;
  this->units.erase(position);
}

void Units::onUnitDiscover(BWAPI::Unit unit)
{
  this->onUnitComplete(unit);
}

Unit* Units::find(BWAPI::Unit unit)
{
  auto position = this->units.find(unit);
  if (position == this->units.end())
    return nullptr;
  return position->second;
}

void Units::printAssignments()
{
  for (auto& item: this->units)
    item.second->printAssignment();
}

void Units::initMap()
{
  int width = BWAPI::Broodwar->mapWidth();
  int height = BWAPI::Broodwar->mapHeight();
  this->map.resize(width);
  for (int32_t x = 0; x < width; ++x)
    this->map[x].resize(height);

  // I have to make sure that minerals and geysers are initialized first, so I can properly
  // assign the mineral/geyser units when creating base data structures
  for (BWAPI::Unit unit: BWAPI::Broodwar->getStaticNeutralUnits())
    this->onUnitComplete(unit);
}

Unit* Units::findOrThrow(BWAPI::Unit unit)
{
  auto position = this->units.find(unit);
  if (position == this->units.end())
    LOG_AND_ABORT("Trying to find unknown " + unit->getType().getName());
  return position->second;
}
