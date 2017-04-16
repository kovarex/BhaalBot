#include <DelayedExitChecker.hpp>

DelayedExitChecker::DelayedExitChecker(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

void DelayedExitChecker::onFrame()
{
  if (this->ticksToExit > 0)
    --this->ticksToExit;
}

void DelayedExitChecker::exitDelayed(uint32_t time)
{
  this->ticksToExit = time + 1;
  this->exitScheduled = true;
}

bool DelayedExitChecker::shouldExit() const
{
  return this->exitScheduled && this->ticksToExit == 0;
}
