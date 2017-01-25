/*  Implementation of default behaviours */
#include "GroupController.hpp"
#include "Group.hpp"
#include "Unit.hpp"
#include <Log.hpp>

void GroupController::setTarget(BWAPI::Unit target)
{ 
  this->target = target; 
  this->updateAction();
}

void GroupController::setTargetPosition(BWAPI::Position position)
{ 
  this->target = position; 
  this->updateAction();
}

void GroupController::setObjective(GroupObjective objective)
{ 
  this->objective = objective; 
  this->updateAction();
}

void GroupController::updateAction()
{
  switch (this->objective)
  {
    case GroupObjective::NONE: 
      this->actionNone();
      break;
    case GroupObjective:: MOVE:
      this->actionMove();
      break;
    case GroupObjective::ATTACK_MOVE:
      this->actionAttackMove();
      break;
    case GroupObjective::ATTACK_TARGET:
      this->actionAttackTarget();
      break;
    case GroupObjective::HOLD:
      this->actionHold();
      break;
    case GroupObjective::DEFEND:
      this->actionDefend();
      break;
    case GroupObjective::KITE:
      this->actionKite();
      break;
    case GroupObjective::FLEE:
      this->actionFlee();
      break;
    case GroupObjective::GROUP:
      this->actionGroup();
      break;
    default:
      this->actionNone();
  }
}

void GroupController::actionMove(void)
{
  BWAPI::Position position;
  for (Unit* unit : this->owner.getUnits())
  {
    position = this->target.getPosition();
    unit->move(position);
  }
  LOG_INFO("moving to position %u, %u", position.x, position.y);
}

void GroupController::actionAttackTarget(void)
{
  for (Unit* unit : this->owner.getUnits())
    if (BWAPI::Unit unit = this->target.getUnit())
      unit->attack(unit);
    else
      unit->attack(this->target.getPosition());
  LOG_INFO("attack target");
}

void GroupController::actionAttackMove(void)
{
  BWAPI::Position position = this->target.getPosition();
  for (Unit* unit : this->owner.getUnits())
    unit->attack(position);
  LOG_INFO("attack move %u, %u", position.x, position.y);
}

void GroupController::actionGroup(void)
{
  BWAPI::Position center = this->getGroupCenter();
  for (Unit* unit : this->owner.getUnits())
    unit->move(center);
  LOG_DEBUG("grouping up");
}

BWAPI::Position GroupController::getGroupCenter(void) const
{
  if (!this->owner.getSize())
    return BWAPI::Positions::None;
  BWAPI::Position center;
  for (Unit* unit : this->owner.getUnits())
    center += unit->getPosition();
  center.x /= this->owner.getUnits().size();
  center.y /= this->owner.getUnits().size();
  return center;
}

bool GroupController::isGrouped(double maxCenterDistance) const
{
  if (this->owner.getSize())
    return true;
  BWAPI::Position center = this->getGroupCenter();
  double maxDistance = 0;
  for (Unit* unit : this->owner.getUnits())
  {
    double distance = unit->getPosition().getDistance(center);
    if (distance > maxDistance)
      maxDistance = distance;
  }
  return maxDistance <= maxCenterDistance;
}
