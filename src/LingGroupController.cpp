#include <LingGroupController.hpp>
#include <BhaalBot.hpp>
#include <Unit.hpp>
#include <Group.hpp>
#include <Vector.hpp>
#include <Log.hpp>

LingGroupController::LingGroupController(Group& owner)
  : GroupController(owner)
{}

void LingGroupController::onAdded(Unit* muta)
{
  this->updateAction();
  // TODO assign it target. Or is it done in on Frame?
}


void LingGroupController::onFrame()
{
  switch (this->objective)
  {
    case GroupObjective::NONE:
      break;
    case GroupObjective::MOVE:
      break;
    case GroupObjective::ATTACK_MOVE:
      LOG_INFO("Updating lings");
      for (Unit *ling : owner.getUnits())
      {
        updateLingAttackMoveAction(ling);
      }
      break;
    case GroupObjective::ATTACK_TARGET:
      break;
    case GroupObjective::HOLD:
      break;
    case GroupObjective::DEFEND:
      break;
    case GroupObjective::KITE:
      break;
    case GroupObjective::FLEE:
      break;
    case GroupObjective::GROUP:
      break;
    default:
      break;
  }
}


void LingGroupController::actionAttackMove()
{
  GroupController::actionAttackMove();
  for (Unit *ling : owner.getUnits())
  {
    updateLingAttackMoveAction(ling);
    BWAPI::Unit target = ling->getOrderTarget();
    if (target != nullptr)
      // TODO find why this does not work (ling has attack order but it does not draw anything.)
      BWAPI::Broodwar->drawLineMap(ling->getPosition(), target->getPosition(), BWAPI::Colors::Red);
  }
}


BestTarget LingGroupController::chooseLingTarget(Unit* ling)
{
  BestTarget bestTarget;
  bestTarget.type = LingTargetType::NONE;
  bestTarget.target = nullptr;
  BWAPI::Unitset targets;

  // Get units in range and find the best one to attack
  targets = ling->getUnitsInWeaponRange(BWAPI::WeaponTypes::Claws, BWAPI::Filter::IsEnemy);
  for (BWAPI::Unit unitInRange : targets)
  {
    // one hit kill
    if (unitInRange->getHitPoints() <= BWAPI::UnitTypes::Zerg_Zergling.groundWeapon().damageAmount())
    {
      if (unitInRange->getType() == BWAPI::UnitTypes::Zerg_Drone)
      {
        // best option of all
        bestTarget.target = unitInRange;
        bestTarget.type = LingTargetType::DRONE_1_HIT;
        break;
      }
      if (unitInRange->getType() == BWAPI::UnitTypes::Zerg_Zergling)
      {
        // second to best option
        bestTarget.target = unitInRange;
        bestTarget.type = LingTargetType::LING_1_HIT;
      }
      if (bestTarget.type > LingTargetType::OTHER_1_HIT)
      {
        bestTarget.target = unitInRange;
        bestTarget.type = LingTargetType::OTHER_1_HIT;
      }
    }
    if (bestTarget.type <= LingTargetType::OTHER_1_HIT)
      continue;

    // two hit kill - we know there is no 1 hit found yet.
    if (unitInRange->getHitPoints() <= 2 * BWAPI::UnitTypes::Zerg_Zergling.groundWeapon().damageAmount())
    {
      if (bestTarget.type > LingTargetType::DRONE_2_HIT &&
          unitInRange->getType() == BWAPI::UnitTypes::Zerg_Drone)
      {
        bestTarget.target = unitInRange;
        bestTarget.type = LingTargetType::DRONE_2_HIT;
      }
      else if (unitInRange->getType() == BWAPI::UnitTypes::Zerg_Zergling)
      {
        bestTarget.target = unitInRange;
        bestTarget.type = LingTargetType::LING_2_HIT;
      }
      // we don't care about other 2 hit units for now
      continue;
    }
    if (bestTarget.type <= LingTargetType::LING_2_HIT)
      continue;

    // Unit in range regardless health
    if (bestTarget.type > LingTargetType::LING_IN_RANGE &&
        unitInRange->getType() == BWAPI::UnitTypes::Zerg_Zergling)
    {
      bestTarget.target = unitInRange;
      bestTarget.type = LingTargetType::LING_IN_RANGE;
    }
    else if (unitInRange->getType() == BWAPI::UnitTypes::Zerg_Drone)
    {
      bestTarget.target = unitInRange;
      bestTarget.type = LingTargetType::DRONE_IN_RANGE;
    }
  }

  if (bestTarget.type <= LingTargetType::DRONE_IN_RANGE) // nothing better can be found
    return bestTarget;

  // if we have not found any target in range, we look furher.
  // first find zerglings and pick the nearest
  bestTarget.target = ling->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::CanAttack && !BWAPI::Filter::IsWorker, 200);
  if (bestTarget.target != nullptr)
  {
    bestTarget.type = LingTargetType::LING;
    return bestTarget;
  }

  // now find drones in range
  bestTarget.target = ling->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::IsWorker, 400);
  if (bestTarget.target != nullptr)
  {
    bestTarget.type = LingTargetType::DRONE;
    return bestTarget;
  }

  // now anything at all
  bestTarget.target = ling->getClosestUnit(BWAPI::Filter::IsEnemy, 600);
  if (bestTarget.target != nullptr)
  {
    bestTarget.type = LingTargetType::OTHER;
    return bestTarget;
  }
 
  return bestTarget;
}

void LingGroupController::flee(Unit* ling, int radiusToConsider, double howFar)
{
  // TODO
}

void LingGroupController::updateLingAttackMoveAction(Unit *ling)
{
  // if unit is performing an attack, do not interrupt it.
  // TODO following code is for debugging the attack distance.
  bool attackFrame = ling->isAttackFrame();
  char option = 'a';
  BWAPI::Unit target = ling->getTarget();
  if (!target)
  {
    target = ling->getOrderTarget();
    option = 'b';
  }
  if (!target)
  {
    target = ling->getClosestUnit(BWAPI::Filter::IsEnemy && BWAPI::Filter::CanAttack, 60);
    option = 'c';
  }

  double x;
  if (attackFrame && target)
  {
    x = target->getPosition().getDistance(ling->getPosition());
    max = (x < max) ? max : x;
    min = (x > min) ? min : x;
    x = 0;
    return;
  }
  else if (ling->isAttackFrame())
    return;
  
  // if under cooldown, use the time to move to a useful target. - i think this case does not need special handling, just attack as if we can.
  
  /*
  - repeatedly get units in some distance (radius)
  priority:
  1 hit kill in range
  if almost dead, flee
  2 hit kill in range
  if > 1 attacker, flee
  ling in range,
  drone in range;
  nearest ling with < 2 attackers
  separated drone
  any drone
  */

  BestTarget bestTarget = chooseLingTarget(ling);

  if (bestTarget.type == LingTargetType::NONE)
  {
    // TODO do something about it
    LOG_INFO("no target found");
    return;
  }

  // if it is the same as current target, don't change it
  if (ling->getOrderTarget() == bestTarget.target)
    return;

  // attack 1 hit target
  if (bestTarget.type <= LingTargetType::OTHER_1_HIT)
  {
    ling->attack(bestTarget.target);
    LOG_INFO("attacking 1 hit");
    return;
  }

  // flee on low health
  if (ling->getHitPoints() <= 10)
  {
    LOG_INFO("runnign away, dying");
    flee(ling, 200, 200);
    return;
  }

  // attack 2 hit target
  if (bestTarget.type <= LingTargetType::DRONE_2_HIT)
  {
    LOG_INFO("attacking 2 hit");
    ling->attack(bestTarget.target);
    return;
  }

  // flee when low health and overpowered - units in weapon range + 10%
  //BWAPI::Unitset attackers = ling->getUnitsInWeaponRange(BWAPI::WeaponTypes::Claws, BWAPI::Filter::IsEnemy && BWAPI::Filter::CanAttack);
  BWAPI::Unitset attackers = ling->getUnitsInRadius(32, BWAPI::Filter::IsEnemy && BWAPI::Filter::CanAttack);
  if (ling->getHitPoints() <= 15 && attackers.size() >= 2)
  {
    LOG_INFO("running away, low hp");
    flee(ling, 50, 20);
    return;
  }

  // flee when overpowered - units in weapon range + 10%
  if (attackers.size() >= 3)
  {
    LOG_INFO("running away, too many enemice");
    flee(ling, 50, 20);
    return;
  }

  // attack any unit in range
  if (bestTarget.type <= LingTargetType::DRONE_IN_RANGE)
  {
    LOG_INFO("attacking in range");
    ling->attack(bestTarget.target);
    return;
  }

  // attack anything available
  if (bestTarget.type < LingTargetType::NONE)
  {
    LOG_INFO("attacking from distance");
    ling->attack(bestTarget.target);
    return;
  }

}