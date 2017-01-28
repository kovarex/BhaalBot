#pragma once
#include <Assignment.hpp>
#include <BWAPI.h>
#include <BWEM/bwem.h>
#include <Module.hpp>
class Base;

class ScoutTaskAssignment : public Assignment
{
public:
  ScoutTaskAssignment() {}
  virtual ~ScoutTaskAssignment();
  std::string str() const override { return "Scout"; }
};

class ScoutingManager : public Module
{
public:
  ScoutingManager(ModuleContainer& moduleContainer);
  ~ScoutingManager();
  void assignGroundScout(Unit* unit);
  void onFrame() override;
  Base* baseToScout();
  bool scoutAssigned(Base* base);
  void unassignScout(Unit* unit);
  void orderToScout(BWAPI::UnitType unitType);
  Unit* getScoutCandidate(BWAPI::UnitType unitType);

  class DiscoverScoutingLocationsScoutTask
  {
  public:
    DiscoverScoutingLocationsScoutTask(Base* target, Unit* scout)
      : target(target)
      , scout(scout) {}
    void onFrame();

    Base* target;
    Unit* scout;
    bool finished = false;
  };

  std::vector<Unit*> unassignedGroundScouters;
  std::vector<DiscoverScoutingLocationsScoutTask> scoutTasks;
  class CheckBaseTask
  {
  public:
    CheckBaseTask(Base* target, Unit* scout)
      : target(target)
      , scout(scout) {}
    void onFrame();

    Base* target;
    Unit* scout;
    bool finished = false;
  };
  std::map<BWAPI::UnitType, int> ordersToScout;
  std::vector<CheckBaseTask> baseCheckTasks;
  static constexpr int scoutCooldown = 24 * 60 * 4;
};
