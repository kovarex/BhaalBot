#pragma once
#include <Module.hpp>
class BuildTaskInProgress;

class BuildTasks : public Module
{
public:
  BuildTasks(ModuleContainer& moduleContainer);
  virtual ~BuildTasks();
  void onFrame() override;

  std::vector<BuildTaskInProgress*> buildTasksInProgress;
};
