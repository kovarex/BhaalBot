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
  TaskForce();
  ~TaskForce();
  TaskForce(const TaskForce&) = delete;
  TaskForce(const TaskForce&&) = delete;
  void operator=(const TaskForce&) = delete;

  void addUnit(Unit* unit);
  void assignTaskController(TaskForceController* taskForceController);
  std::vector<Unit*> removeGroup(Group* group);
  std::vector<Unit*> removeAllGroups();
  Group* createGroup();
  void onFrame();
protected:
  virtual void onAdded(Unit*) {}

public:
  std::set<Group*> groups;
  TaskForceController* taskForceController = nullptr;
};
