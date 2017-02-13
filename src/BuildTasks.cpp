#include <BuildTasks.hpp>
#include <BuildTaskInProgress.hpp>

BuildTasks::BuildTasks(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

BuildTasks::~BuildTasks()
{
  for (BuildTaskInProgress* taskInProgress: this->buildTasksInProgress)
    delete taskInProgress;
}

void BuildTasks::onFrame()
{
  for (uint32_t i = 0; i < this->buildTasksInProgress.size();)
  {
    BuildTaskInProgress* buildTaskInProgress = this->buildTasksInProgress[i];
    if (buildTaskInProgress->onFrame())
    {
      delete buildTaskInProgress;
      this->buildTasksInProgress.erase(this->buildTasksInProgress.begin() + i);
    }
    else
      ++i;
  }
}

int BuildTasks::plannedCount(BWAPI::UnitType unitType)
{
  int result = 0;
  for (BuildTaskInProgress* taskInProgress: this->buildTasksInProgress)
    if (taskInProgress->getUnitType() == unitType)
      ++result;
  return result;
}
