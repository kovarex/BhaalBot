#pragma once
#include <BWAPI.h>
#include <Module.hpp>
class Unit;

/** Handler producers (Hatcheries) available for building */
class ProducerManager : public Module
{
public:
  ProducerManager(ModuleContainer& moduleContainer);
  void onUnitComplete(Unit* unit) override;
  Unit* getBestProducer();

  std::vector<Unit*> producers;
};
