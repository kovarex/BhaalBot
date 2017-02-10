#pragma once
#include <Module.hpp>
class BuildTaskInProgress;

class BuildTasks : public Module
{
public:
  BuildTasks(ModuleContainer& moduleContainer);
  virtual ~BuildTasks();
  void onFrame() override;
  int plannedCount(BWAPI::UnitType unitType);

  std::vector<BuildTaskInProgress*> buildTasksInProgress;
};
