#pragma once
class Strategy;

class StrategySelector
{
public:
  StrategySelector() {}

  Strategy* select();
};
