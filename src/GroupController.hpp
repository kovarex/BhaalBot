#pragma once
#include <BWAPI.h>
class Group;
class Unit;

class GroupController
{
public:
  GroupController(Group& owner) : owner(owner) {}
  virtual void onAdded(Unit* unit) {}
  virtual void onRemoved(Unit* unit) {}
  virtual void onFrame() {}
  virtual void setAttackTarget(BWAPI::Unit target) {}
  virtual BWAPI::Unit getAttackTarget() { return nullptr; }

  Group& owner;
};
