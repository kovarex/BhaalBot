#pragma once
#include <BWAPI.h>
#include <Module.hpp>
#include <CostReservation.hpp>
#include <set>

/** Monitors which larvae are reserved to be morphed to something, so I don't morph 2 things into a single larva.
 * It also (Through the MorphTaskInProgress) reserves the money needed to morph the larva. */
class LarvaReservations : public Module
{
public:
  LarvaReservations(ModuleContainer& moduleContainer);
  ~LarvaReservations();
  bool tryToTrain(Unit* hatchery, BWAPI::UnitType targetUnit); // TODO would be nice to have this not only with a hatch, but with a base and global.
  void registerTask(Unit* larva, BWAPI::UnitType targetUnit);
  void onUnitDestroy(Unit* unit);
  void onFrame() override;
  int32_t reservedLarvas(Unit* hatch);
  bool isResrved(Unit* larva);

  /** Monitors the stage of morphing from the order to morph being given until the larva becomes an egg and starts morphing.
   * If the train command fails (which happens sometimes), it will be ordered again after some short period of time.
   * Once the egg is created, the morphing is monitored by the MorphingUnits class */
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
