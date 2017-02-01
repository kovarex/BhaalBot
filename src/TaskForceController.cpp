#include <BhaalBot.hpp>
#include <Bases.hpp>
#include <Base.hpp>
#include <Group.hpp>
#include <TaskForceController.hpp>
#include <TaskForce.hpp>
#include <Unit.hpp>
#include <MutaGroupController.hpp>
#include <LingGroupController.hpp>
#include <log.hpp>
#include <DroneDefenseGroupController.hpp>
#include <Player.hpp>

#define MAX_DISTANCE_TO_GROUP_NEW_LINGS 200 // currently in pixels
#define MAX_LING_GROUP_ERROR 50
#define DISTANCE_FROM_ENEMY_TO_START_COMBAT 300

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
          LOG_INFO("Adding ling to reinforcements");
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
      LOG_INFO("Starting new reinforecements group");
      this->lingReinforementGroups.push_back(this->owner.createGroup());
      Group* group = this->lingReinforementGroups.back();
      group->add(unit);
      LingGroupController* controller = new LingGroupController(*group);
      group->assignController(controller);
      controller->setTargetPosition(this->enemyBase ?
                                    this->enemyBase->getCenter() :
                                    BWAPI::Position(BWAPI::Broodwar->mapWidth() * 16, BWAPI::Broodwar->mapHeight() * 16));
      controller->setObjective(GroupObjective::MOVE);
    }
  }
  else
    this->groundGroup->add(unit);
}

void AttackTaskForceController::onFrame()
{
  if (!this->enemyBase)
  {
    if (bhaalBot->bases.startingBase)
    {
      this->enemyBase = bhaalBot->bases.getEnemyBaseClosestTo(bhaalBot->bases.startingBase->getCenter());
      if (this->enemyBase)
      {
        for (auto* theGroup: {&this->lingReinforementGroups, &this->lingCombatGroups})
          for (Group* group: *theGroup)
            group->getController()->setTargetPosition(this->enemyBase->getCenter());
      }
    }
  }

  //--------------------------------------------- MUTAS
  for (Group* group: this->mutaGroups)
  {
    if (group->getTarget()->isZero())
    {
      BWAPI::Position groupPosition = group->getPosition();
      Unit* bestCandidate = nullptr;
      double bestCandidateDistance = 0;
      for (Player* player: bhaalBot->players.enemies)
        for (Unit* unit: player->units)
        {
          double distance = unit->getDistance(groupPosition);
          if (bestCandidate == nullptr ||
              distance < bestCandidateDistance)
          {
            bestCandidate = unit;
            bestCandidateDistance = distance;
          }
        }
      if (bestCandidate)
        group->setAttackTarget(bestCandidate);
    }
  }
  //--------------------------------------------- LINGZ
  // transform reinforcements into combat groups, if they are close enough to enemy base
  for (auto it = this->lingReinforementGroups.begin(); it < this->lingReinforementGroups.end();)
  {
    if (this->enemyBase != nullptr &&
        this->enemyBase->getCenter().getDistance((*it)->getPosition()) < DISTANCE_FROM_ENEMY_TO_START_COMBAT)
    {
      LOG_INFO("Switching ling from reinforcements to combat, grouping");
      this->lingCombatGroups.push_back((*it));
      (*it)->getController()->setObjective(GroupObjective::GROUP);
      it = lingReinforementGroups.erase(it);
    }
    else
      ++it;
  }

  for (Group* group : this->lingCombatGroups)
  {
    if (group->getController()->getObjective() == GroupObjective::GROUP &&
      group->getController()->isGrouped(MAX_LING_GROUP_ERROR))
    {
      LOG_INFO("Grouped, attacking");
      group->getController()->setTargetPosition(this->enemyBase->getCenter());
      group->getController()->setObjective(GroupObjective::ATTACK_MOVE);
    }
  }

  // if sunken exists, assign lings not under direct attack to kill it
  // if drones run too much, assign lings to kill pool
  // handle combat groups - maybe just let them do their do.
}

TaskForceController::TaskForceController(TaskForce& owner)
  : owner(owner)
{}
