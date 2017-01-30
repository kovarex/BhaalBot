#pragma once
#include <Module.hpp>
#include <set>
class Unit;

/** Manages the (our) Unit objects. Provides mapping from the BWAPI::Unit pointer to our unit wrapper. */
class Units
{
public:
  ~Units();
  Unit* onUnitComplete(BWAPI::Unit unit);
  void onUnitDestroy(BWAPI::Unit unit);
  Unit* findOrThrow(BWAPI::Unit unit);
  Unit* find(BWAPI::Unit unit);
  void printAssignments();
  const std::set<Unit*>& getUnits() const { return this->unitSet; }

private:
  std::map<BWAPI::Unit, Unit*> units;
  std::set<Unit*> unitSet;
};
