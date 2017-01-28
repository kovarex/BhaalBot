#include <LingGroupController.hpp>
#include <BhaalBot.hpp>
#include <Unit.hpp>
#include <Group.hpp>
#include <Vector.hpp>

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
  for (Unit *ling : owner.getUnits())
  {
    updateLingAttackMoveAction(ling);
  }
}


BestTarget LingGroupController::chooseLingTarget(Unit* ling)
{
  BestTarget bestTarget;
  bestTarget.type = LingTargetType::NONE;
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
    if (bestTarget.type >= LingTargetType::OTHER_1_HIT)
      continue;

    // two hit kill - we know there is no 1 hit found yet.
    if (unitInRange->getHitPoints() <= 2 * BWAPI::UnitTypes::Zerg_Zergling.groundWeapon().damageAmount())
    {
      if (bestTarget.type > LingTargetType::DRONE_2_HIT &&
          bestTarget.target->getType() == BWAPI::UnitTypes::Zerg_Drone)
      {
        bestTarget.target = unitInRange;
        bestTarget.type = LingTargetType::DRONE_2_HIT;
      }
      else if (bestTarget.target->getType() == BWAPI::UnitTypes::Zerg_Zergling)
      {
        bestTarget.target = unitInRange;
        bestTarget.type = LingTargetType::LING_2_HIT;
      }
      // we don't care about other 2 hit units for now
      continue;
    }
    if (bestTarget.type >= LingTargetType::LING_2_HIT)
      continue;

    // Unit in range regardless health
    if (bestTarget.type > LingTargetType::LING_IN_RANGE &&
        bestTarget.target->getType() == BWAPI::UnitTypes::Zerg_Zergling)
    {
      bestTarget.target = unitInRange;
      bestTarget.type = LingTargetType::LING_IN_RANGE;
    }
    else if (bestTarget.target->getType() == BWAPI::UnitTypes::Zerg_Drone)
    {
      bestTarget.target = unitInRange;
      bestTarget.type = LingTargetType::DRONE_IN_RANGE;
    }
  }

  if (bestTarget.type >= LingTargetType::DRONE_IN_RANGE)
    return bestTarget;

  // if we have not found any target in range, we look furher.
  // first find zerglings and pick the nearest
  double distance = INT_MAX;
  targets = ling->getUnitsInRadius(200, BWAPI::Filter::IsEnemy && BWAPI::Filter::CanAttack && !BWAPI::Filter::IsWorker);
  for (BWAPI::Unit unitInRadius : targets)
  {
    double currentDistance = unitInRadius->getPosition().getDistance(ling->getPosition());
    if (currentDistance < distance)
    {
      bestTarget.target = unitInRadius;
      distance = currentDistance;
    }
    bestTarget.type = LingTargetType::LING;
    return bestTarget;
  }

  // now find drones in range
  targets = ling->getUnitsInRadius(400, BWAPI::Filter::IsEnemy && BWAPI::Filter::IsWorker);
  for (BWAPI::Unit unitInRadius : targets)
  {
    double currentDistance = unitInRadius->getPosition().getDistance(ling->getPosition());
    if (currentDistance < distance)
    {
      bestTarget.target = unitInRadius;
      distance = currentDistance;
    }
    bestTarget.type = LingTargetType::DRONE;
  }

  // now anything at all
  targets = ling->getUnitsInRadius(600, BWAPI::Filter::IsEnemy);
  for (BWAPI::Unit unitInRadius : targets)
  {
    double currentDistance = unitInRadius->getPosition().getDistance(ling->getPosition());
    if (currentDistance < distance)
    {
      bestTarget.target = unitInRadius;
      distance = currentDistance;
    }
    bestTarget.type = LingTargetType::OTHER;
    return bestTarget;
  }

  return bestTarget;
}

void LingGroupController::updateLingAttackMoveAction(Unit *ling)
{
  // if unit is performing an attack, do not interrupt it.
  if (ling->isAttackFrame())
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

  // if it is the same as current target, don't change it
  if (ling->getOrderTarget() == bestTarget.target)
    return;

  // attack 1 hit target
  if (bestTarget.type <= LingTargetType::OTHER_1_HIT)
  {
    ling->attack(bestTarget.target);
    return;
  }

  // flee on low health
  if (ling->getHitPoints() <= 10)
  {
    // flee(ling); TODO
    return;
  }

  // attack 2 hit target
  if (bestTarget.type <= LingTargetType::DRONE_2_HIT)
  {
    ling->attack(bestTarget.target);
    return;
  }

  // flee when low health and overpowered - units in weapon range + 10%
  BWAPI::Unitset attackers = ling->getUnitsInRadius(1, BWAPI::Filter::IsEnemy && BWAPI::Filter::CanAttack);
  if (ling->getHitPoints() <= 15 && attackers.size() >= 2)
  {
    // flee(ling); TODO
    return;
  }

  // flee when overpowered - units in weapon range + 10%
  if (attackers.size() >= 3)
  {
    // flee(ling); TODO
    return;
  }

  // attack any unit in range
  if (bestTarget.type <= LingTargetType::DRONE_IN_RANGE)
  {
    ling->attack(bestTarget.target);
    return;
  }

  // attack anything available
  if (bestTarget.type < LingTargetType::NONE)
  {
    ling->attack(bestTarget.target);
    return;
  }

}