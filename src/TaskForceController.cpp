#include <BhaalBot.hpp>
#include <Group.hpp>
#include <TaskForceController.hpp>
#include <TaskForce.hpp>
#include <Unit.hpp>
#include <MutaGroupController.hpp>
#include <LingGroupController.hpp>

#define MAX_DISTANCE_TO_GROUP_NEW_LINGS 300 // currently in pixels
#define MAX_LING_GROUP_ERROR 100
#define DISTANCE_FROM_ENEMY_TO_START_COMBAT 500

AttackTaskForceController::AttackTaskForceController(TaskForce& owner)
  : TaskForceController(owner)
{
  std::vector<Unit*> units = owner.removeAllGroups();
  this->groundGroup = owner.createGroup();
  for (Unit* unit: units)
    this->assignUnit(unit);
  enemyBasePos = { 2200, 200 }; // TODO update
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
    Group* group = this->owner.createGroup();
    group->assignController(new MutaGroupController(*group));
    group->add(unit);
    this->mutaGroups.push_back(group);
  }
  else if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling)
  {
    // Try to join the ling to a gathering new group, or create new group for him.
    bool lingAdded = false;
    if (!this->lingReinforementGroups.empty())
    {
      // if there is a reinforcement group clouse enough, join it and make it regroup.
      for (Group* group : this->lingReinforementGroups)
      {
        if (unit->getPosition().getDistance(group->getPosition()) < MAX_DISTANCE_TO_GROUP_NEW_LINGS)
        {
          group->add(unit);
          if (!group->getController()->isGrouped(MAX_LING_GROUP_ERROR))
          {
            group->getController()->setObjective(GroupObjective::GROUP);
          }
          lingAdded = true;
          break;
        }
      }
    }
    // if ling still not handled, create a new group for him.
    if (this->lingReinforementGroups.empty() || !lingAdded)
    {
      this->lingReinforementGroups.push_back(this->owner.createGroup());
      Group* group = this->lingReinforementGroups.back();
      group->add(unit);
      LingGroupController* controller = new LingGroupController(*group);
      group->assignController(controller);
      controller->setTargetPosition(enemyBasePos);
      controller->setObjective(GroupObjective::MOVE);
    }
  }
  else
    this->groundGroup->add(unit);
}

void AttackTaskForceController::onFrame()
{
  //--------------------------------------------- MUTAS
  for (Group* group: this->mutaGroups)
  {
    if (group->getTarget() == nullptr)
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
  //--------------------------------------------- LINGZ
  // transform reinforcements into combat groups, if they are close enough to enemy base
  for (auto it = this->lingReinforementGroups.begin(); it < this->lingReinforementGroups.begin(); ++it)
  {
    if (enemyBasePos.getDistance((*it)->getPosition()) > DISTANCE_FROM_ENEMY_TO_START_COMBAT)
    {
      this->lingCombatGroups.push_back((*it));
      (*it)->getController()->setTargetPosition({2300, 200});
      (*it)->getController()->setObjective(GroupObjective::ATTACK_MOVE);
      lingReinforementGroups.erase(it);
    }
  }

  // if sunken exists, assign lings not under direct attack to kill it
  // if drones run too much, assign lings to kill pool
  // handle combat groups - maybe just let them do their do.
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
