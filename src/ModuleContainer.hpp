#pragma once
#include <vector>
#include <BWAPI.h>
class Module;
class Unit;

class ModuleContainer
{
public:
  void add(Module* module);
  void remove(Module* module);

  void onUnitComplete(Unit* unit);
  void onUnitCreate(Unit* unit);
  void onUnitDiscover(Unit* unit);
  void onUnitMorph(Unit* unit, BWAPI::UnitType from);
  void onFrame();
  void onStart();
  void onUnitDestroy(Unit* unit);
  void onUnitIdle(Unit* unit);
  void onEnd(bool isWinner);
private:
  std::vector<Module*> modules;
};
