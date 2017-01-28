#include <DefendTaskForceController.hpp>
#include <DroneDefenseGroupController.hpp>
#include <Group.hpp>
#include <TaskForce.hpp>
#include <Unit.hpp>

DefendTaskForceController::DefendTaskForceController(TaskForce& owner, Base* base)
  : base(base)
  , TaskForceController(owner)
{
  std::vector<Unit*> units = this->owner.removeAllGroups();
  this->theGroup = this->owner.createGroup();
  this->droneDefenseGroup = this->owner.createGroup();
  this->droneDefenseGroup->assignController(new DroneDefenseGroupController(*this->droneDefenseGroup, this->base));
  for (Unit* unit: units)
    this->assignUnit(unit);
}

void DefendTaskForceController::assignUnit(Unit* unit)
{
  if (unit->canGather())
    this->droneDefenseGroup->add(unit);
  else
    this->theGroup->add(unit);
}
