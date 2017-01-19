#pragma once
#include <Module.hpp>
#include <MutaGroupManager.hpp>

class FightManager : public Module
{
public:
  FightManager(ModuleContainer& moduleContainer);
  virtual void onUnitComplete(Unit* unit) override;
  virtual void onFrame() override;
  Unit* freeUnit(BWAPI::UnitType unitType);

  std::vector<MutaGroupManager> mutaGroups;
  std::vector<Unit*> otherUnits;
};
