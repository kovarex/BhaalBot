#pragma once
#include <BWAPI.h>
class Group;
class Unit;
class UnitMemoryInfo;

enum class GroupObjective
{
  NONE,   // no objective set 
  MOVE,   // move to the target
  ATTACK, // attack the target
  HOLD,   // hold ground
  DEFEND, // defend the target
  KITE,   // kite the target
  FLEE,   // move away from danger
  GROUP   // group up
  // TODO add advanced objectives, such as guard area, attack unit type, ...
};

class GroupController
{
public:
  GroupController(Group& owner, GroupObjective objective = GroupObjective::NONE) : owner(owner), objective(objective) {}
  virtual void onAdded(Unit* unit) {}
  virtual void onRemoved(Unit* unit) {}
  virtual void onFrame();
  virtual void setAttackTarget(BWAPI::Unit target) {} // TODO rename to setTarget as the group does not need to attack it.
  virtual void setTargetPosition(BWAPI::Position position) { this->targetPosition = position; }
  virtual UnitMemoryInfo* getAttackTarget() { return nullptr; }
  virtual void setObjective(GroupObjective objective) { this->objective = objective; }
  virtual GroupObjective getObjective(void) const { return this->objective; }
  virtual BWAPI::Position getPosition(void) const { return this->getGroupCenter(); }
  virtual BWAPI::Position getGroupCenter(void) const;
  virtual bool isGrouped(double maxCenterDistance) const;

  virtual void preAction() {} //!< actions performed onFrame before objective action is performed.
  virtual void postAction() {} //!< actions performed onFrame after objective action is performed.
  // behaviour implementations
  // TODO implement the remaining behaviours within this class
  virtual void actionNone() {}
  virtual void actionMove();
  virtual void actionAttack();
  virtual void actionHold() {}
  virtual void actionDefend() {}
  virtual void actionKite() {}
  virtual void actionFlee() {}
  virtual void actionGroup();

  Group& owner;
  GroupObjective objective; // this, in combination with target, is the basic command given to the group by it's TaskForce.
  BWAPI::Position targetPosition;
};
