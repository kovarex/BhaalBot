#include <Unit.hpp>
#include <BhaalBot.hpp>
#include <Log.hpp>

Unit::Unit(BWAPI::Unit bwapiUnit)
  : bwapiUnit(bwapiUnit)
  , lastSeenUnitType(this->getType())
  , lastSeenPosition(this->bwapiUnit->getPosition())
  , player(bhaalBot->players.findPlayer(this->getBWAPIPlayer()))
#ifdef DEBUG
  , name(this->getType().getName())
#endif
{
  this->registerOnMap();
}

Unit::~Unit()
{
  this->unregsiterFromMap();
  delete this->assignment;
  for (UnitTarget* target: this->targetingMe)
    target->unit = nullptr;
  delete this->memoryInfo;
}

void Unit::printAssignment()
{
  if (this->assignment)
    BWAPI::Broodwar->drawTextMap(this->getPosition(), "%s", this->assignment->str().c_str());
}

void Unit::assign(Assignment* assignment)
{
  delete this->assignment;
  this->assignment = assignment;
  if (assignment)
    assignment->unit = this;
}

void Unit::updatePosition()
{
  if (this->bwapiUnit->isVisible())
  {
    BWAPI::Position currentPosition = this->bwapiUnit->getPosition();
    if (BWAPI::TilePosition(this->lastSeenPosition) != BWAPI::TilePosition(currentPosition))
    {
      this->unregsiterFromMap();
      this->lastSeenPosition = currentPosition;
      this->registerOnMap();
    }
  }
}

void Unit::registerOnMap()
{
  BWAPI::TilePosition tilePosition(this->lastSeenPosition);
  Units::TileInfo& tileInfo = bhaalBot->units.map[tilePosition.x][tilePosition.y];
  this->nextOnTile = tileInfo.units;
  if (this->nextOnTile)
    this->nextOnTile->previousOnTile = this;
  this->previousOnTile = nullptr;
  tileInfo.units = this;
  bhaalBot->discoveredMemory.onAdd(this);
}

void Unit::unregsiterFromMap()
{
  bhaalBot->discoveredMemory.onRemove(this);
  if (this->previousOnTile)
  {
    this->previousOnTile->nextOnTile = this->nextOnTile;
    if (this->nextOnTile)
      this->nextOnTile->previousOnTile = this->previousOnTile;
  }
  else
  {
    BWAPI::TilePosition tilePosition(this->lastSeenPosition);
    Units::TileInfo& tileInfo = bhaalBot->units.map[tilePosition.x][tilePosition.y];
#ifdef DEBUG
    if (tileInfo.units != this)
      LOG_AND_ABORT("Map registration mismatch.");
#endif
    tileInfo.units = this->nextOnTile;
    if (this->nextOnTile)
      this->nextOnTile->previousOnTile = nullptr;
  }
}

std::string Unit::shortenUnitName(const std::string& name)
{
  if (name.substr(0, 5) == "Zerg_")
    return name.substr(5, name.size() - 5);
  if (name.substr(0, 8) == "Protoss_")
    return name.substr(8, name.size() - 8);
  if (name.substr(0, 7) == "Terran_")
    return name.substr(7, name.size() - 7);
  return name;
}

std::string Unit::getName() const
{
  return Unit::shortenUnitName(this->getType().getName());
}

void Unit::addTarget(UnitTarget* target)
{
  this->targetingMe.push_back(target);
}

void Unit::removeTarget(UnitTarget* target)
{
  for (auto it = this->targetingMe.begin(); it != this->targetingMe.end(); ++it)
    if (*it == target)
    {
      this->targetingMe.erase(it);
      return;
    }
  LOG_AND_ABORT("Trying to remove target that is not here.");
}
