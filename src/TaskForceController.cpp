#include <Group.hpp>
#include <TaskForceController.hpp>
#include <TaskForce.hpp>
#include <Unit.hpp>

AttackTaskForceController::AttackTaskForceController(TaskForce& owner)
  : TaskForceController(owner)
{
  std::vector<Unit*> units = owner.removeAllGroups();
  owner.groups.insert(this->groundGroup = new Group());
  for (Unit* unit: units)
    this->assignUnit(unit);
}

void AttackTaskForceController::assignUnit(Unit* unit)
{
  if (unit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk)
  {
    for (Group* group: this->mutaGroups)
      if (group->getUnits().count() < 11)
      {
        group->add(unit);
        return;
      }
    Group* group = owner.createGroup();
    group->add(unit);
  }
  else
    this->groundGroup->add(unit);
}

DefendTaskForceController::DefendTaskForceController(TaskForce& owner)
{
  std::vector<Unit*> units = owner.removeAllGroups();
  owner.groups.insert(this->theGroup = new Group());
  for (Unit* unit: units)
    this->assignUnit(unit);
}

void DefendTaskForceController::assignUnit(Unit* unit)
{
  this->theGroup->add(unit);
}
