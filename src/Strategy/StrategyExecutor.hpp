#pragma once
#include <Module.hpp>
class Strategy;

class StrategyExecutor : public Module
{
public:
  StrategyExecutor(ModuleContainer& moduleContainer);
  void onStart() override;
  void onEnd(bool isWinner) override;
  void onFrame() override;

  Strategy* currentStrategy = nullptr;
};
