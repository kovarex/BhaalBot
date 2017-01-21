#pragma once
class Group;
class TaskForce;
class Unit;

class TaskForceController
{
public:
  TaskForceController(TaskForce& owner);
  virtual void assignUnit(Unit* unit) = 0;
  
protected:
  TaskForce& owner;
};

class AttackTaskForceController : public TaskForceController
{
public:
  AttackTaskForceController(TaskForce& owner);
  void assignUnit(Unit* unit) override;

  std::vector<Group*> mutaGroups;
  Group* groundGroup;
};

class DefendTaskForceController : public TaskForceController
{
public:
  DefendTaskForceController(TaskForce& owner);
  void assignUnit(Unit* unit) override;

  Group* theGroup;
};
