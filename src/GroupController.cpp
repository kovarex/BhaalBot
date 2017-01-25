/*  Implementation of default behaviours */
#include "GroupController.hpp"
#include "Group.hpp"
#include "Unit.hpp"

void GroupController::onFrame()
{
  this->preAction();

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

  this->postAction();
}

void GroupController::actionMove(void)
{
  for (Unit* unit : this->owner.getUnits())
    unit->move(this->target.getPosition());
}

void GroupController::actionAttackTarget(void)
{
  for (Unit* unit : this->owner.getUnits())
    if (BWAPI::Unit unit = this->target.getUnit())
      unit->attack(unit);
    else
      unit->attack(this->target.getPosition());
}

void GroupController::actionAttackMove(void)
{
  for (Unit* unit : this->owner.getUnits())
    unit->attack(this->target.getPosition());
}

void GroupController::actionGroup(void)
{
  BWAPI::Position center = this->getGroupCenter();
  for (Unit* unit : this->owner.getUnits())
    unit->move(center);
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
