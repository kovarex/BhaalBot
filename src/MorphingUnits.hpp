#pragma once
#include <BWAPI.h>
#include <Module.hpp>

class MorphingUnits : Module
{
public:
  MorphingUnits(ModuleContainer& moduleContainer);
  void onUnitMorph(Unit* unit, BWAPI::UnitType from) override;
  uint32_t getPlannedCount(BWAPI::UnitType unitType);
  void addPlannedMorph(BWAPI::UnitType unitType) { this->plannedMorphs[unitType]++; }
  void removePlannedMorph(BWAPI::UnitType unitType) { this->plannedMorphs[unitType]--; }

private:
  uint32_t getMorphingCount(BWAPI::UnitType unitType);
  uint32_t getPlannedMorphsCount(BWAPI::UnitType unitType);
  std::map<BWAPI::UnitType, uint32_t> counts;
  std::map<BWAPI::UnitType, uint32_t> plannedMorphs;
};
