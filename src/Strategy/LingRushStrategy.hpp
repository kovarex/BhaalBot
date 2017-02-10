#pragma once
#include <Strategy/Strategy.hpp>

/** Simple X pool rush */
class LingRushStrategy : public Strategy
{
public:
  LingRushStrategy(int dronesWhenPool);

  static bool initInstances();
  void onStart() override;
  void onFrame() override;

  int dronesWhenPool;
  static bool initialized;
};
