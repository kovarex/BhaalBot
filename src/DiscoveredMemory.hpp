#pragma once
#include <BWAPI.h>
#include <Module.hpp>
class EnemyUnitTarget;

class UnitMemoryInfo
{
public:
  UnitMemoryInfo()
    : position(BWAPI::Positions::None)
    , unitType(BWAPI::UnitTypes::None)
    , lastSeenTick(0) {}
  UnitMemoryInfo(BWAPI::Unit unit);
  ~UnitMemoryInfo();
  void update(BWAPI::Unit unit);
  void addTarget(EnemyUnitTarget* target);
  void removeTarget(EnemyUnitTarget* target);

  BWAPI::Unit unit = nullptr;
  BWAPI::Position position;
  BWAPI::UnitType unitType;
  int lastSeenTick;

  enum class State
  {
    Normal,
    WasntFoundOnItsLastSeenLocation
  };

  State state = State::Normal;
  std::vector<EnemyUnitTarget*> targetingMe;
};

class DiscoveredMemory : public Module
{
public:
  class EnemyUnitTarget;
  DiscoveredMemory(ModuleContainer& moduleContainer);
  ~DiscoveredMemory();

  void onFrame() override;
  void onForeignUnitComplete(BWAPI::Unit unit) override;
  void onForeignUnitDestroy(BWAPI::Unit unit) override;
  void addUnit(BWAPI::Unit unit);
  void onAdd(BWAPI::Unit unit);
  void onRemove(std::map<BWAPI::Unit, UnitMemoryInfo*>::iterator unit);
  UnitMemoryInfo* getUnit(BWAPI::Unit);

  std::map<BWAPI::Unit, UnitMemoryInfo*> units;
};
