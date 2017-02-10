#pragma once
#include <Module.hpp>
class Strategy;

/** Uses the StrategySelector at the start of the game to select starting strategy.
 * Performs the strategy transition logic */
class StrategyExecutor : public Module
{
public:
  StrategyExecutor(ModuleContainer& moduleContainer);
  void onStart() override;
  void onEnd(bool isWinner) override;
  void onFrame() override;

  Strategy* currentStrategy = nullptr;
};
