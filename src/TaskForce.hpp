#pragma once
#include <BWAPI.h>
#include <set>
class Unit;
class Group;
class GroupController;
class TaskForceController;

class TaskForce
{
public:
  TaskForce(TaskForceController* taskForceController);
  ~TaskForce();
  TaskForce(const TaskForce&) = delete;
  TaskForce(const TaskForce&&) = delete;
  void operator=(const TaskForce&) = delete;

  void addUnit(Unit* unit);
  std::vector<Unit*> removeGroup(Group* group);
  std::vector<Unit*> removeAllGroups();
  Group* createGroup();
protected:
  virtual void onAdded(Unit*) {}

public:
  std::set<Group*> groups;
  TaskForceController* taskForceController;
};
