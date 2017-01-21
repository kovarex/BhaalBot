#include <BhaalBot.hpp>
#include <Group.hpp>
#include <TaskForceController.hpp>
#include <TaskForce.hpp>
#include <Unit.hpp>
#include <MutaGroupController.hpp>

AttackTaskForceController::AttackTaskForceController(TaskForce& owner)
  : TaskForceController(owner)
{
  std::vector<Unit*> units = owner.removeAllGroups();
  this->groundGroup = owner.createGroup();
  for (Unit* unit: units)
    this->assignUnit(unit);
}

void AttackTaskForceController::assignUnit(Unit* unit)
{
  if (unit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk)
  {
    for (Group* group: this->mutaGroups)
      if (group->getUnits().size() < 11)
      {
        group->add(unit);
        return;
      }
    Group* group = owner.createGroup();
    group->assignController(new MutaGroupController(*group));
    group->add(unit);
    this->mutaGroups.push_back(group);
  }
  else
    this->groundGroup->add(unit);
}

void AttackTaskForceController::onFrame()
{
  for (Group* group: this->mutaGroups)
  {
    if (group->getAttackTarget() == nullptr)
    {
      BWAPI::Position groupPosition = group->getPosition();
      BWAPI::Unit bestCandidate = nullptr;
      double bestCandidateDistance = 0;
      for (auto& item: bhaalBot->discoveredMemory.units)
      {
        double distance = item.first->getDistance(groupPosition);
        if (bestCandidate == nullptr ||
            distance < bestCandidateDistance)
        {
          bestCandidate = item.first;
          bestCandidateDistance = distance;
        }
      }
      if (bestCandidate)
        group->setAttackTarget(bestCandidate);
    }
  }
}

DefendTaskForceController::DefendTaskForceController(TaskForce& owner)
  : TaskForceController(owner)
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

TaskForceController::TaskForceController(TaskForce& owner)
  : owner(owner)
{}
