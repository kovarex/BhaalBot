#include <Unit.hpp>
#include <BhaalBot.hpp>
#include <Log.hpp>

Unit::Unit(BWAPI::Unit bwapiUnit)
  : bwapiUnit(bwapiUnit)
  , lastSeenUnitType(this->getType())
  , player(bhaalBot->players.findPlayer(this->getBWAPIPlayer()))
{

}

Unit::~Unit()
{
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
