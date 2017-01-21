#pragma once
#include <BWAPI.h>
class ModuleContainer;
class Unit;

/** Interface of anything that needs to get the BWAPI::evenets. */
class Module
{
public:
  Module(ModuleContainer& container);
  ~Module();
  Module(const Module& other) = delete;
  Module(const Module&& other) = delete;
  void operator=(const Module& other) = delete;

  virtual void onUnitComplete(Unit*) {}
  virtual void onForeignUnitComplete(BWAPI::Unit) {}
  virtual void onUnitMorph(Unit*) {}
  virtual void onFrame() {}
  virtual void onStart() {}
  virtual void onUnitDestroy(Unit*) {}
  virtual void onForeignUnitDestroy(BWAPI::Unit) {}

  ModuleContainer& owner;
};
