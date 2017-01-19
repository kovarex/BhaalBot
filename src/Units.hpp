#pragma once
#include <Module.hpp>
class Unit;

class Units
{
public:
  Unit* onUnitComplete(BWAPI::Unit unit);
  void onUnitDestroy(BWAPI::Unit unit);
  Unit* findOrThrow(BWAPI::Unit unit);
  Unit* find(BWAPI::Unit unit);
  void printAssignments();

private:
  std::map<BWAPI::Unit, Unit*> units;
};
