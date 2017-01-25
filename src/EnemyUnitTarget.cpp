#include <EnemyUnitTarget.hpp>
#include <BhaalBot.hpp>

EnemyUnitTarget::EnemyUnitTarget(BWAPI::Unit unit)
{
  UnitMemoryInfo* unitMemoryInfo = bhaalBot->discoveredMemory.getUnit(unit);
  if (unitMemoryInfo)
    unitMemoryInfo->addTarget(this);
}

void EnemyUnitTarget::operator=(BWAPI::Unit unit)
{
  this->clear();
  this->data = bhaalBot->discoveredMemory.getUnit(unit);
  if (this->data)
    this->data->addTarget(this);
}

EnemyUnitTarget::~EnemyUnitTarget()
{
  this->clear();
}

void EnemyUnitTarget::clear()
{
  if (this->data == nullptr)
    return;
  this->data->removeTarget(this);
  this->data = nullptr;
}

BWAPI::Position EnemyUnitTarget::getPosition() const
{
  return this->data ? this->data->position : BWAPI::Positions::None;
}

BWAPI::Unit EnemyUnitTarget::getUnit() const
{
  if (this->data == nullptr)
    return nullptr;
  return this->data->unit;
}
