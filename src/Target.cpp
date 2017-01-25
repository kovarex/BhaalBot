#include <Target.hpp>

void Target::operator=(BWAPI::Position position)
{
  this->unit.clear();
  this->position = position;
}

void Target::operator=(BWAPI::Unit unit)
{
  this->position = BWAPI::Positions::None;
  this->unit = unit;
}

bool Target::isZero() const
{
  return !this->unit.isZero() ||
    this->position != BWAPI::Positions::None;
}

BWAPI::Position Target::getPosition() const
{
  if (!this->unit.isZero())
    return this->unit.getPosition();
  return this->position;
}

BWAPI::Unit Target::getUnit() const
{
  return this->unit.getUnit();
}

void Target::clear()
{
  this->unit.clear();
  this->position = BWAPI::Positions::None;
}
