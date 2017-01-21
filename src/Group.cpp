#include <Group.hpp>
#include <GroupController.hpp>
#include <Unit.hpp>

void Group::add(Unit* unit)
{
  this->units.insert(unit);
  if (this->controller)
    this->controller->onAdded(unit);
  if (unit->getAssignment() == nullptr)
    unit->assign(new SimpleGroupAssignment(this));
}

void Group::remove(Unit* unit)
{
  this->units.erase(unit);
  if (this->controller)
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
  return nullptr;
}

void Group::assignController(GroupController* groupController)
{
  delete this->controller;
  this->controller = groupController;
}

void Group::setAttackTarget(BWAPI::Unit target)
{
  if (this->controller)
    this->controller->setAttackTarget(target);
}

BWAPI::Unit Group::getAttackTarget()
{
  if (this->controller)
    return this->controller->getAttackTarget();
  return nullptr;
}

BWAPI::Position Group::getPosition() const
{
  int xSum = 0, ySum = 0;
  for (Unit* unit: this->units)
  {
    xSum += unit->getPosition().x;
    ySum += unit->getPosition().y;
  }
  return BWAPI::Position(xSum / this->units.size(), ySum / this->units.size());
}

SimpleGroupAssignment::~SimpleGroupAssignment()
{
  this->group->remove(unit);
}
