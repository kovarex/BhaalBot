#pragma once
#include <Strategy/Strategy.hpp>

class EarlyMarinePush : public Strategy
{
public:
  EarlyMarinePush();
  static bool initInstances();
  void onStart() override;
  void onFrame() override;

  int firstGate;
  static bool initialized;
};
