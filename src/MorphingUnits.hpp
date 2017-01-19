#pragma once
#include <BWAPI.h>
#include <Module.hpp>

class MorphingUnits : Module
{
public:
  MorphingUnits(ModuleContainer& moduleContainer);
  void onUnitMorph(Unit* unit) override;
  uint32_t getMorphingCount(BWAPI::UnitType unitType);

private:
  class MorfingUnit
  {
    BWAPI::Unit unit;

  };
  std::map<BWAPI::UnitType, uint32_t> counts;
};
