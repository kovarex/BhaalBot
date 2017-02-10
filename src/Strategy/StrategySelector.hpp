#pragma once
class Strategy;

/** Select the starting strategy.
 * Currently it selects randomly (or by config) strategy for current matchup.
 * It should select it based on the success statistics and opponent later on. */
class StrategySelector
{
public:
  StrategySelector() {}

  Strategy* select();
};
