#pragma once
#include <Assignment.hpp>
#include <BWAPI.h>
#include <set>
#include <Target.hpp>
class Group;
class GroupController;
class Unit;
class UnitMemoryInfo;
class TargetWithPath;

class SimpleGroupAssignment : public Assignment
{
public:
  SimpleGroupAssignment(Group* group) : group(group) {}
  ~SimpleGroupAssignment();
  std::string str() const override { return "SimpleGroup"; }

  Group* group;
};

class Group
{
public:
  ~Group();
  void add(Unit* unit);
  void remove(Unit* unit);
  const std::set<Unit*>& getUnits() const { return this->units; }
  void onFrame();
  Unit* getUnit(BWAPI::UnitType unitType);
  void assignController(GroupController* groupController);
  GroupController* getController(void) { return this->controller;  }
  void setAttackTarget(Unit* target); // TODO this should not be needed - orders should be given to controller.
  const Target* getTarget();
  BWAPI::Position getPosition() const;
  int getSize(void) { return this->units.size(); }

private:
  std::set<Unit*> units;
  GroupController* controller = nullptr;
};
