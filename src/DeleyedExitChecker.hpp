#pragma once
#include <Module.hpp>

class DelayedExitChecker : public Module
{
public:
  DelayedExitChecker(ModuleContainer& moduleContainer);
  virtual void onFrame() override;
  void exitDelayed(uint32_t time);
  bool shouldExit() const;

private:
  uint32_t ticksToExit = 0;
  bool exitScheduled = false;
};
