#pragma once
#include <Assignment.hpp>
#include <BWAPI.h>
#include <BWEM/bwem.h>
class Base;
class Unit;
class BaseHarvestingController;

class MineralHarvestingAssignment : public Assignment
{
public:
  MineralHarvestingAssignment(BaseHarvestingController* base) : base(base) {}
  virtual ~MineralHarvestingAssignment();
  std::string str() const override { return "Minerals"; }

private:
  BaseHarvestingController* base;
};

class GasHarvestingAssignment : public Assignment
{
public:
  GasHarvestingAssignment(BaseHarvestingController* base) : base(base) {}
  virtual ~GasHarvestingAssignment();
  std::string str() const override { return "Gas"; }
private:
  BaseHarvestingController* base;
};

class BaseHarvestingController
{
public:
  struct Mineral
  {
    Mineral(BWAPI::Unit mineral) : mineral(mineral) {}
    BWAPI::Unit mineral;
    std::vector<Unit*> miners;
  };

  BaseHarvestingController(Unit* baseUnit, Base* base);
  ~BaseHarvestingController();
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
