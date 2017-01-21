#pragma once
#include <set>
class GroupController;
class Unit;

class Group
{
public:
  void add(Unit* unit);
  void remove(Unit* unit);
  const std::set<Unit*>& getUnits() const { return this->units; }
private:
  std::set<Unit*> units;
  GroupController* controller;
};
