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
  void registerTask(Unit* larvae, BWAPI::UnitType targetUnit);
  void onFrame() override;
  int32_t reservedLarvas(Unit* hatch);
  bool isResrved(Unit* larva);

  class MorphTaskInProgress : public CostReservationItem
  {
  public:
    MorphTaskInProgress(Unit* larvae, BWAPI::UnitType targetUnit);
    std::string str() const override;
    bool morhphingStarted() const;

    Unit* larvaeToBeUsed;
    BWAPI::UnitType targetUnit;
    Unit* parentHatchery;
  };

private:
  std::vector<MorphTaskInProgress*> morphTasks;
  std::map<Unit*, uint32_t> reservedByProducers;
  std::set<Unit*> larvaesRegistered;
  std::map<BWAPI::UnitType, uint32_t> plannedUnits;
};
