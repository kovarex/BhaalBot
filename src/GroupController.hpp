#pragma once
class Group;
class Unit;

class GroupController
{
public:
  GroupController(Group& owner) : owner(owner) {}
  virtual void onAdded(Unit* unit) {}
  virtual void onRemoved(Unit* unit) {}
  virtual void onFrame() {}

  Group& owner;
};
