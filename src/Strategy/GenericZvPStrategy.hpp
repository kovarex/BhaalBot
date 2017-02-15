#pragma once
#include <Strategy/Strategy.hpp>

class GenericZvPStrategy : public Strategy
{
  public:
  GenericZvPStrategy();

  static bool initInstances();
  void onStart() override;
  void onFrame() override;

  static bool initialized;
};
