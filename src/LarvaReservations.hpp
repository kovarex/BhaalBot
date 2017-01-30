#pragma once
#include <BWAPI.h>
#include <Module.hpp>
#include <CostReservation.hpp>
#include <set>

class LarvaReservations : public Module
{
public:
  LarvaReservations(ModuleContainer& moduleContainer);
  ~LarvaReservations();
  bool tryToTrain(Unit* hatchery, BWAPI::UnitType targetUnit);
  void registerTask(Unit* larva, BWAPI::UnitType targetUnit);
  void onUnitDestroy(Unit* unit);
  void onFrame() override;
  int32_t reservedLarvas(Unit* hatch);
  bool isResrved(Unit* larva);

  class MorphTaskInProgress : public CostReservationItem
  {
  public:
    MorphTaskInProgress(Unit* larva, BWAPI::UnitType targetUnit);
    std::string str() const override;
    bool morhphingStarted() const;
    void onFrame() const;

    Unit* larvaToBeUsed;
    BWAPI::UnitType targetUnit;
    Unit* parentHatchery;
    int tickOfLastOrder;
  };

private:
  std::vector<MorphTaskInProgress*> morphTasks;
  std::map<Unit*, uint32_t> reservedByProducers; /**< The amount of larva reserved for these hatcheries */
  std::set<Unit*> larvasRegistered;
};
