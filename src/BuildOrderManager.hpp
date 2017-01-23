#pragma once
#include <BWAPI.h>
#include <Cost.hpp>
#include <BuildOrderExecutor.hpp>
#include <Module.hpp>
class BhaalBot;

class BuildOrderManager : Module
{
public:
  BuildOrderManager(ModuleContainer& moduleContainer);
  ~BuildOrderManager();

  virtual void onFrame() override;
  void onUnitComplete(BWAPI::Unit unit);
  void add(BuildOrder* buildOrder);

  std::map<std::string, BuildOrder*> buildOrders;
  BuildOrderExecutor executor;
};
