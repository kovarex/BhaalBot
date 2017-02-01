#pragma once
#include <BWAPI.h>
#include <Module.hpp>
#include <set>
class UnitTarget;

class UnitMemoryInfo
{
public:
  UnitMemoryInfo()
    : position(BWAPI::Positions::None)
    , lastSeenTick(0) {}
  UnitMemoryInfo(Unit* unit);
  void update(Unit* unit);

  BWAPI::Position position;
  int lastSeenTick;

  enum class State
  {
    Normal,
    WasntFoundOnItsLastSeenLocation
  };

  State state = State::Normal;
};

class DiscoveredMemory : public Module
{
public:
  class UnitTarget;
  DiscoveredMemory(ModuleContainer& moduleContainer);

  void onFrame() override;
  void onUnitComplete(Unit* unit) override;
  void onUnitDestroy(Unit* unit) override;
  void addUnit(Unit* unit);
  void onAdd(Unit* unit);
  void onRemove(Unit* unit);
};
