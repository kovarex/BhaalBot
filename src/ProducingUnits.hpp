#pragma once
#include <set>
#include <Module.hpp>
class Unit;

/** Units being trained (by the production queue)
 * this is applicable only for Protoss and Terran production, the Zerg production is controlled by the MorphingUnits class.*/
class ProducingUnits : public Module
{
public:
  ProducingUnits(ModuleContainer& moduleContainer);
  void onUnitCreate(Unit* unit);
  void onUnitComplete(Unit* unit);
  void onUnitDestroy(Unit* unit);
  Unit* deduceProducer(Unit* unit);
  Unit* getProducingUnit(Unit* producer);

  class UnitToBeProducedData
  {
  public:
    UnitToBeProducedData() {}
    UnitToBeProducedData(Unit* producer) : producer(producer) {}

    Unit* producer = nullptr;
    bool assignedByTheOrder = false;
  };

  std::map<Unit*, UnitToBeProducedData> unitsBeingProduced;
};
