#pragma once
#include <BWAPI.h>
#include <Module.hpp>
#include <set>
class UnitTarget;

class UnitMemoryInfo
{
public:
  UnitMemoryInfo()
    : lastSeenTick(0) {}
  UnitMemoryInfo(Unit* unit);
  void update(Unit* unit);

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
  void onAdd(Unit* unit);
  void onRemove(Unit* unit);
};
