#include <BhaalBot.hpp>
#include <Strategy/Strategy.hpp>
#include <Strategy/StrategyExecutor.hpp>
#include <Strategy/StrategySelector.hpp>
#include <Log.hpp>

StrategyExecutor::StrategyExecutor(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

void StrategyExecutor::onStart()
{
  this->currentStrategy = bhaalBot->strategySelector.select();
  if (this->currentStrategy == nullptr)
  {
    LOG_WARNING("Couldn't find any strategy for this matchup");
    bhaalBot->deleyedExitChecker.exitDelayed(24 * 5);
    return;
  }
  this->currentStrategy->registerTo(bhaalBot->moduleContainer);
  this->currentStrategy->onStart();
}

void StrategyExecutor::onEnd(bool)
{
  if (this->currentStrategy)
    this->currentStrategy->unregisterFrom();
}

void StrategyExecutor::onFrame()
{
  if (this->currentStrategy &&
      this->currentStrategy->switchOrder != nullptr)
  {
    LOG_NOTICE("Switching to %s", this->currentStrategy->switchOrder->name.c_str());
    this->currentStrategy->unregisterFrom();
    this->currentStrategy = this->currentStrategy->switchOrder;
    this->currentStrategy->onStart();
    this->currentStrategy->registerTo(bhaalBot->moduleContainer);
  }
}
