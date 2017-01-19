#pragma once
#include <BWAPI.h>
#include <set>
#include <Module.hpp>

class BuildingPlaceabilityHelper : public Module
{
public:
  BuildingPlaceabilityHelper(ModuleContainer& moduleContainer);
  bool canBuild(BWAPI::UnitType unit, BWAPI::TilePosition position, Unit* builder);
  void registerBuild(BWAPI::UnitType unit, BWAPI::TilePosition position);
  void unRegisterBuild(BWAPI::UnitType unit, BWAPI::TilePosition position);
  void onFrame() override;

  std::set<BWAPI::TilePosition> tilesPlannedToTake;
};
