#pragma once
#include <Module.hpp>

class Overmind : public Module
{
public:
  Overmind(ModuleContainer& moduleContainer);

  void onUnitComplete(Unit* unit) override;
  void onUnitIdle(Unit* unit) override;
  void onUnitDestroy(Unit* unit) override;
  void onFrame();
private:
  void assignUnit(Unit* unit);
};
