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
  FLEE    // move away from danger
  // TODO add advanced objectives, such as guard area, attack unit type, ...
};

class GroupController
{
public:
  GroupController(Group& owner, GroupObjective objective = GroupObjective::NONE) : owner(owner), objective(objective) {}
  virtual void onAdded(Unit* unit) {}
  virtual void onRemoved(Unit* unit) {}
  virtual void onFrame() {}
  virtual void setAttackTarget(BWAPI::Unit target) {} // TODO rename to setTarget as the group does not need to attack it.
  virtual UnitMemoryInfo* getAttackTarget() { return nullptr; }
  virtual void setObjective(GroupObjective objective) { this->objective = objective; }
  virtual GroupObjective getObjective(void) const { return this->objective; }

  Group& owner;
  GroupObjective objective; // this, in combination with target, is the basic command given to the group by it's TaskForce.
};
