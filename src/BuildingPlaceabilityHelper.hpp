#pragma once
#include <BWAPI.h>
#include <set>
#include <Module.hpp>

/** Tool to check buildability of buildings and to avoid planning to build more buildings at the same location. */
class BuildingPlaceabilityHelper : public Module
{
public:
  BuildingPlaceabilityHelper(ModuleContainer& moduleContainer);
  bool canBuild(BWAPI::UnitType unit, BWAPI::TilePosition position, Unit* builder);
  void registerBuild(BWAPI::UnitType unit, BWAPI::TilePosition position);
  void unRegisterBuild(BWAPI::UnitType unit, BWAPI::TilePosition position);
  void onFrame() override;

private:
  std::set<BWAPI::TilePosition> tilesPlannedToTake;
};
