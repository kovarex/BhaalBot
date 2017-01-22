#pragma once
#include <Assignment.hpp>
#include <BWAPI.h>
#include <BWEM/bwem.h>
class Base;
class Unit;
class BaseHarvestingManager;

class MineralHarvestingAssignment : public Assignment
{
public:
  MineralHarvestingAssignment(BaseHarvestingManager* base) : base(base) {}
  virtual ~MineralHarvestingAssignment();
  std::string str() const override { return "Minerals"; }

private:
  BaseHarvestingManager* base;
};

class GasHarvestingAssignment : public Assignment
{
public:
  GasHarvestingAssignment(BaseHarvestingManager* base) : base(base) {}
  virtual ~GasHarvestingAssignment();
  std::string str() const override { return "Gas"; }
private:
  BaseHarvestingManager* base;
};

class BaseHarvestingManager
{
public:
  struct Mineral
  {
    Mineral(BWAPI::Unit mineral) : mineral(mineral) {}
    BWAPI::Unit mineral;
    std::vector<Unit*> miners;
  };

  BaseHarvestingManager(Unit* baseUnit, Base* base);
  void assignMiner(Unit* unit);
  void update();
  void drawDebug();
  void onUnitComplete(Unit* unit);
  Mineral* getBestMineral();
  uint32_t smallestMineralSaturation() const;
  uint32_t biggestMineralSaturation() const;
  Unit* freeLeastNeededWorker();
  BWAPI::Position againstMinerals();
  
  Unit* baseUnit;
  std::vector<Mineral> minerals;
  struct Geyser
  {
    Geyser(BWAPI::Unit geyser) : geyser(geyser) {}

    enum class State
    {
      Free,
      OrderToBuildOverGiven,
      BuildingFacility,
      Ready
    };
    BWAPI::Unit geyser;
    State state = State::Free;
    std::vector<Unit*> miners;
  };
  std::vector<Geyser> geysers;
  Base* base;
private:
  friend class MineralHarvestingAssignment;
  friend class GasHarvestingAssignment;
  void unassignFromMinerals(Unit* unit);
  void unassignFromGas(Unit* unit);
};
