#include <Group.hpp>
#include <GroupController.hpp>
#include <Unit.hpp>

void Group::add(Unit* unit)
{
  this->units.insert(unit);
  this->controller->onAdded(unit);
  if (unit->getAssignment() == nullptr)
    unit->assign(new SimpleGroupAssignment(this));
}

void Group::remove(Unit* unit)
{
  this->units.erase(unit);
  this->controller->onRemoved(unit);
}

void Group::onFrame()
{
  if (this->controller)
    this->controller->onFrame();
}

Unit* Group::getUnit(BWAPI::UnitType unitType)
{
  for (Unit* unit: this->units)
    if (unit->getType() == unitType)
      return unit;
}

SimpleGroupAssignment::~SimpleGroupAssignment()
{
  this->group->remove(unit);
}
