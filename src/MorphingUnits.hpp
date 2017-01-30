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

  void planMorphOf(Unit* unit) { this->thingsToBeMorphed.insert(unit); }
  bool isScheduledToBeMorphed(Unit* unit) { return this->thingsToBeMorphed.count(unit) > 0; }

private:
  uint32_t getMorphingCount(BWAPI::UnitType unitType);
  uint32_t getPlannedMorphsCount(BWAPI::UnitType unitType);
  std::map<BWAPI::UnitType, uint32_t> counts;
  std::map<BWAPI::UnitType, uint32_t> plannedMorphs;
  std::set<Unit*> thingsToBeMorphed;
};
