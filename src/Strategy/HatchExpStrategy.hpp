#pragma once
#include <Strategy/Strategy.hpp>

/** Transition between early start and midgame strategy.
 * It doesn't do anything but the hatch build, so it is supposed to be switched to
 * some midgame strategy (to be done).
 * TODO: Switch based on opponent
         For starters we might want to have Mass ling, hydra, muta */
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
