#pragma once
#include <Module.hpp>
#include <MutaGroupController.hpp>
class TaskForce;

class FightManager : public Module
{
public:
  FightManager(ModuleContainer& moduleContainer);
  ~FightManager();
  virtual void onUnitComplete(Unit* unit) override;
  virtual void onFrame() override;
  Unit* getUnit(BWAPI::UnitType unitType);

  std::vector<TaskForce*> taskForces;
  TaskForce* attack;
};
