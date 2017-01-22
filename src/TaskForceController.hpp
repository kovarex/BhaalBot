#pragma once
class Group;
class TaskForce;
class Unit;

class TaskForceController
{
public:
  TaskForceController(TaskForce& owner);
  virtual void assignUnit(Unit* unit) = 0;
  virtual void onFrame() {}
  
protected:
  TaskForce& owner;
};

class AttackTaskForceController : public TaskForceController
{
public:
  AttackTaskForceController(TaskForce& owner);
  void assignUnit(Unit* unit) override;
  void onFrame() override;

  std::vector<Group*> mutaGroups;

  std::vector<Group*> lingReinforementGroups;  // group used to move lings to the enemy base
  std::vector<Group*> lingCombatGroups;         // general offensive combat against lings, drones etc.
  std::vector<Group*> lingSunkenKillGroups;     // group assigned to kill a specific sunken colony
  Group* lingPoolKillGroup;                     // group assigned to kill spawning pool

  Group* groundGroup;
};

class DefendTaskForceController : public TaskForceController
{
public:
  DefendTaskForceController(TaskForce& owner);
  void assignUnit(Unit* unit) override;

  Group* theGroup;
};
