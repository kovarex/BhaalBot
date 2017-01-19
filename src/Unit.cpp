#include <Unit.hpp>
#include <BhaalBot.hpp>

Unit::~Unit()
{
  delete this->assignment;
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
