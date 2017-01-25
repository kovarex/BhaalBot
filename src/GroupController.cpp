/*  Implementation of default behaviours */
#include "GroupController.hpp"
#include "Group.hpp"
#include "Unit.hpp"

void GroupController::onFrame()
{
  preAction();

  switch (this->objective)
  {
    case GroupObjective::NONE: 
      actionNone();
      break;
    case GroupObjective:: MOVE:
      actionMove();
      break;
    case GroupObjective::ATTACK:
      actionAttack();
      break;
    case GroupObjective::HOLD:
      actionHold();
      break;
    case GroupObjective::DEFEND:
      actionDefend();
      break;
    case GroupObjective::KITE:
      actionKite();
      break;
    case GroupObjective::FLEE:
      actionFlee();
      break;
    case GroupObjective::GROUP:
      actionGroup();
      break;
    default:
      actionNone();
  }

  postAction();
}

void GroupController::actionMove(void)
{
  for (Unit* unit : this->owner.getUnits())
  {
    unit->move(this->targetPos);
  }
}

void GroupController::actionAttack(void)
{
  for (Unit* unit : this->owner.getUnits())
  {
    unit->attack(this->targetPos);
  }
}

void GroupController::actionGroup(void)
{
  BWAPI::Position center = this->getGroupCenter();
  for (Unit* unit : this->owner.getUnits())
  {
    unit->move(center);
  }
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