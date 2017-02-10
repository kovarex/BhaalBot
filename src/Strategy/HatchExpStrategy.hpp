#pragma once
#include <Strategy/Strategy.hpp>

class HatchExpStrategy : public Strategy
{
public:
  HatchExpStrategy(int hatchCount);

  static bool initInstances();
  void onStart() override;
  void onFrame() override;

  int hatchCount;
  static bool initialized;
};
