#include <DangerZones.hpp>
#include <algorithm>

DangerZones::DangerZones(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

void DangerZones::onStart()
{
  this->width = BWAPI::Broodwar->mapWidth();
  this->height = BWAPI::Broodwar->mapHeight();
  this->data.resize(this->width);
  for (int32_t x = 0; x < this->width; ++x)
    this->data[x].resize(this->height);
}

void DangerZones::onFrame()
{
  for (int32_t x = 0; x < this->width; ++x)
    for (int32_t y = 0; y < this->height; ++y)
      if (this->data[x][y].danger != 0)
        BWAPI::Broodwar->drawTextMap(BWAPI::Position(BWAPI::TilePosition(x, y)), "%d", this->data[x][y]);
}

void DangerZones::addDanger(BWAPI::Position centerPosition, int32_t radius, int32_t value)
{
  BWAPI::TilePosition centerPositionTiled(centerPosition);
  BWAPI::TilePosition position;
  for (position.x = std::max(0, centerPositionTiled.x - radius); position.x < std::min(this->width, centerPositionTiled.x + radius); ++position.x)
    for (position.y = std::max(0, centerPositionTiled.y - radius); position.y < std::min(this->width, centerPositionTiled.y + radius); ++position.y)
      if (position.getApproxDistance(centerPositionTiled) <= radius)
        this->data[position.x][position.y].danger += value;
}

void DangerZones::removeDanger(BWAPI::Position centerPosition, int32_t radius, int32_t value)
{
  BWAPI::TilePosition centerPositionTiled(centerPosition);
  BWAPI::TilePosition position;
  for (position.x = std::max(0, centerPositionTiled.x - radius); position.x < std::min(this->width, centerPositionTiled.x + radius); ++position.x)
    for (position.y = std::max(0, centerPositionTiled.y - radius); position.y < std::min(this->width, centerPositionTiled.y + radius); ++position.y)
      if (position.getApproxDistance(centerPositionTiled) <= radius)
        this->data[position.x][position.y].danger -= value;
}
