#pragma once
#include <map>
#include <Module.hpp>
#include <CostReservation.hpp>

/** Monitors which queues have additional production tasks in progress.
 * Mainly to avoid ordering (for example) two zealots from the same gate the same tick when the other one should be ordered in the second one 
 * This is applicable only for Protoss and Terran productions, for Zerg, the LarvaReservation is used instead. */
class ProductionQueueReservations : public Module
{
public:
  ProductionQueueReservations(ModuleContainer& moduleContainer);
  ~ProductionQueueReservations();
  bool tryToTrain(Unit* producer, BWAPI::UnitType targetUnit);
  void onUnitCreate(Unit* unit) override;
  int plannedCount(BWAPI::UnitType unitType);
  void onFrame() override;

  class ProductionOrderInProgress : public CostReservationItem
  {
  public:
    ProductionOrderInProgress(Unit* producer, BWAPI::UnitType targetUnit);
    std::string str() const override;

    Unit* producer;
    BWAPI::UnitType targetUnit;
    int tickOfLastOrder;
  };
  std::map<Unit*, std::vector<ProductionOrderInProgress*>> ordersInProgress;
};
