#pragma once
#include <Module.hpp>

class Overmind : public Module
{
public:
  Overmind(ModuleContainer& moduleContainer);

  void onUnitComplete(Unit* unit) override;
  void onFrame();
};
