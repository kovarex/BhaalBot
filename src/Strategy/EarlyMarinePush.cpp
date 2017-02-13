#include <BhaalBot.hpp>
#include <BuildOrder.hpp>
#include <Strategy/EarlyMarinePush.hpp>
#include <StringUtil.hpp>

bool EarlyMarinePush::initialized = EarlyMarinePush::initInstances();

EarlyMarinePush::EarlyMarinePush()
  : Strategy("Early marine push", BWAPI::Races::Terran, BWAPI::Races::None, false)
{}

bool EarlyMarinePush::initInstances()
{
  new EarlyMarinePush();
  return true;
}

void EarlyMarinePush::onStart()
{
  BuildOrder* buildOrder = new BuildOrder();
  buildOrder->add(4, BWAPI::UnitTypes::Terran_SCV);
  buildOrder->add(BWAPI::UnitTypes::Terran_Barracks);
  buildOrder->add(1, BWAPI::UnitTypes::Terran_SCV);
  buildOrder->add(BWAPI::UnitTypes::Terran_Barracks);
  buildOrder->add(1, BWAPI::UnitTypes::Terran_Supply_Depot);
  buildOrder->add(new SwitchToAutomaticSupplyBuilding());
  buildOrder->add(100, BWAPI::UnitTypes::Terran_Marine);

  bhaalBot->buildOrderManager.executor.startBuildOrder(buildOrder);
}

void EarlyMarinePush::onFrame()
{
  //TODO middle game strategy
}
