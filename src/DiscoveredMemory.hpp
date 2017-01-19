#pragma once
#include <BWAPI.h>
#include <Module.hpp>

class DiscoveredMemory : public Module
{
public:
  class UnitMemoryInfo
  {
  public:
    UnitMemoryInfo()
      : position(BWAPI::Positions::None)
      , unitType(BWAPI::UnitTypes::None)
      , lastSeenTick(0) {}
    UnitMemoryInfo(BWAPI::Unit unit);

    BWAPI::Position position;
    BWAPI::UnitType unitType;
    int lastSeenTick;
  };

  DiscoveredMemory(ModuleContainer& moduleContainer);

  void onFrame() override;
  void onForeignUnitComplete(BWAPI::Unit unit) override;
  void onForeignUnitDestroy(BWAPI::Unit unit) override;
  void addUnit(BWAPI::Unit unit);
  void onAdd(BWAPI::Unit unit);
  void onRemove(std::map<BWAPI::Unit, UnitMemoryInfo>::iterator unit);

  std::map<BWAPI::Unit, UnitMemoryInfo> units;
};
