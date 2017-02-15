#include <BhaalBot.hpp>
#include <BuildOrder.hpp>
#include <BuildOrderManager.hpp>
#include <Strategy/GenericZvPStrategy.hpp>
#include <StringUtil.hpp>
#include <Player.hpp>
#include <UnitCompositionSelector/ZvPUnitCompositionSelector.hpp>

bool GenericZvPStrategy::initialized = GenericZvPStrategy::initInstances();

GenericZvPStrategy::GenericZvPStrategy()
  : Strategy("Generic ZvP", BWAPI::Races::Zerg, BWAPI::Races::Protoss, false)
{}

bool GenericZvPStrategy::initInstances()
{
  new GenericZvPStrategy();
  return true;
}

void GenericZvPStrategy::onStart()
{
  this->buildOrder = new BuildOrder();
  buildOrder->add(new SetUnitComposition(new ZvPUnitCompositionSelector()));
  buildOrder->add(new SwitchToAutomaticSupplyBuilding());
  bhaalBot->buildOrderManager.executor.startBuildOrder(buildOrder);
}

void GenericZvPStrategy::onFrame()
{
  // TODO
}
