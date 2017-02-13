#pragma once
#include <Strategy/Strategy.hpp>

class GatePushStrategy : public Strategy
{
public:
  GatePushStrategy(int firstGate);
  static bool initInstances();
  void onStart() override;
  void onFrame() override;

  int firstGate;
  static bool initialized;
};
