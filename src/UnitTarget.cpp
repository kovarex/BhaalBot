#include <UnitTarget.hpp>
#include <BhaalBot.hpp>
#include <Unit.hpp>

UnitTarget::UnitTarget(Unit* unit)
  : unit(unit)
{
  if (this->unit)
    this->unit->addTarget(this);
}

void UnitTarget::operator=(const UnitTarget& other)
{
  this->clear();
  this->unit = other.unit;
  if (this->unit)
    this->unit->addTarget(this);
}

void UnitTarget::operator=(Unit* unit)
{
  this->clear();
  this->unit = unit;
  if (this->unit)
    this->unit->addTarget(this);
}

UnitTarget::~UnitTarget()
{
  this->clear();
}

void UnitTarget::clear()
{
  if (this->unit == nullptr)
    return;
  this->unit->removeTarget(this);
  this->unit = nullptr;
}

BWAPI::Position UnitTarget::getPosition() const
{
  if (this->unit == nullptr)
    return BWAPI::Positions::None;
  if (this->unit->isVisible())
    return this->unit->getPosition();
  if (this->unit->memoryInfo)
    return this->unit->lastSeenPosition;
  return BWAPI::Positions::None;
}
