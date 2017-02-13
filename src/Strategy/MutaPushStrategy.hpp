#pragma once
#include <Strategy/Strategy.hpp>

/** Techs to mutas and tries to build a muta wave.
 * Future responzibilities: Guard defense and sunks
 *                          */
class MutaExpStrategy : public Strategy
{
public:
  MutaExpStrategy(int hatchCount);

  static bool initInstances();
  void onStart() override;
  void onFrame() override;

  int hatchCount;
  static bool initialized;
};
