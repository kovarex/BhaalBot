#pragma once
#include <Module.hpp>
class Unit;

/** Manages the (our) Unit objects. Provides mapping from the BWAPI::Unit pointer to our unit wrapper. */
class Units
{
public:
  Unit* onUnitComplete(BWAPI::Unit unit);
  void onUnitDestroy(BWAPI::Unit unit);
  Unit* findOrThrow(BWAPI::Unit unit);
  Unit* find(BWAPI::Unit unit);
  void printAssignments();
  const std::map<BWAPI::Unit, Unit*>& getUnits() const { return this->units; }

private:
  std::map<BWAPI::Unit, Unit*> units;
};
