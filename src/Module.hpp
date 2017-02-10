#pragma once
#include <BWAPI.h>
class ModuleContainer;
class Unit;

/** Interface of anything that needs to get the BWAPI::evenets. */
class Module
{
public:
  Module(ModuleContainer& container);
  Module();
  virtual ~Module();
  Module(const Module& other) = delete;
  Module(const Module&& other) = delete;
  void operator=(const Module& other) = delete;
  void registerTo(ModuleContainer& container);
  void unregisterFrom();

  virtual void onUnitComplete(Unit*) {}
  virtual void onUnitMorph(Unit*, BWAPI::UnitType from) { (void)(from); }
  virtual void onFrame() {}
  virtual void onStart() {}
  virtual void onUnitDestroy(Unit*) {}
  virtual void onUnitIdle(Unit* unit) {}
  virtual void onEnd(bool isWinner) { (void)(isWinner); }

  ModuleContainer* owner = nullptr;
};
