#pragma once
#include <BWAPI.h>
#include <BWEM/bwem.h>
#include <Module.hpp>

class ScoutingManager : public Module
{
public:
  ScoutingManager(ModuleContainer& moduleContainer);
  void assignGroundScout(Unit* unit);
  void onFrame() override;
  const BWEM::Base* getClosestBase(BWAPI::Position position);
  const BWEM::Base* baseToScout();
  bool scoutAssigned(const BWEM::Base* base);
  void onUnitDestroy(Unit* unit);
  BWAPI::Position enemyMainBase();

  class DiscoverScoutingLocationsScoutTask
  {
  public:
    DiscoverScoutingLocationsScoutTask(const BWEM::Base* target, Unit* scout)
      : target(target)
      , scout(scout) {}
    void onFrame();

    const BWEM::Base* target;
    Unit* scout;
    bool finished = false;
  };

  enum class StartingLocationState
  {
    Self,
    Enemy,
    Empty,
    Unknown
  };
  std::map<const BWEM::Base*, StartingLocationState> startingLocations;
  std::vector<Unit*> unassignedGroundScouters;
  std::vector<DiscoverScoutingLocationsScoutTask> scoutTasks;
  class BaseScoutingData
  {
  public:
    int lastCheckedTick = 0;
  };
  class CheckBaseTask
  {
  public:
    CheckBaseTask(const BWEM::Base* target, Unit* scout)
      : target(target)
      , scout(scout) {}
    void onFrame();

    const BWEM::Base* target;
    Unit* scout;
    bool finished = false;
  };
  const BWEM::Base* startingBase = nullptr;
  std::map<const BWEM::Base*, BaseScoutingData> bases;
  std::vector<CheckBaseTask> baseCheckTasks;
  bool initialised = false;
  static constexpr int scoutCooldown = 24 * 60 * 4;
};
