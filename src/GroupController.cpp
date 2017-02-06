/*  Implementation of default behaviours */
#include "GroupController.hpp"
#include "Group.hpp"
#include "Unit.hpp"
#include <Log.hpp>

void GroupController::setTarget(Unit* target)
{ 
  this->target.path.clear();
  this->target.target = target; 
  this->updateAction();
}

void GroupController::setTargetPosition(BWAPI::Position position)
{ 
  this->target.path.clear();
  this->target.target = position; 
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
      //LOG_INFO("Group MOVE - update action");
      this->actionMove();
      break;
    case GroupObjective::ATTACK_MOVE:
      //LOG_INFO("Group ATTACK MOVE - update action");
      this->actionAttackMove();
      break;
    case GroupObjective::ATTACK_TARGET:
      //LOG_INFO("Group ATTACK TARGET - update action");
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

void GroupController::actionMove()
{
  BWAPI::Position position;
  for (Unit* unit : this->owner.getUnits())
  {
    position = this->target.target.getPosition();
    unit->move(position);
  }
  LOG_INFO("moving to position %u, %u", position.x, position.y);
}

void GroupController::actionAttackTarget()
{
  for (Unit* unit : this->owner.getUnits())
    if (Unit* unit = this->target.target.getUnit())
      unit->attack(unit);
    else
      unit->attack(this->target.target.getPosition());
  LOG_INFO("attack target");
}

void GroupController::actionAttackMove()
{
  BWAPI::Position position = this->target.target.getPosition();
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

BWAPI::Position GroupController::getGroupCenter() const
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
  if (!this->owner.getSize())
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
