#include <BhaalBot.hpp>
#include <BuildOrder.hpp>
#include <Strategy/GatePushStrategy.hpp>
#include <StringUtil.hpp>

bool GatePushStrategy::initialized = GatePushStrategy::initInstances();

GatePushStrategy::GatePushStrategy(int firstGate)
  : Strategy(ssprintf("%d/%d gate", firstGate, firstGate + 1), BWAPI::Races::Protoss, BWAPI::Races::None, false)
  , firstGate(firstGate)
{}

bool GatePushStrategy::initInstances()
{
  //for (int i = 8; i < 10; ++i)
  //    new GatePushStrategy(i);
  // Lets do only 9/10 gate for starters
  new GatePushStrategy(9);
  return true;
}

void GatePushStrategy::onStart()
{
  BuildOrder* buildOrder = new BuildOrder();
  buildOrder->add(firstGate > 8 ? 4 : 3, BWAPI::UnitTypes::Protoss_Probe);
  buildOrder->add(BWAPI::UnitTypes::Protoss_Pylon);
  buildOrder->add(firstGate - (firstGate > 8 ? 8 : 7), BWAPI::UnitTypes::Protoss_Probe);
  buildOrder->add(BWAPI::UnitTypes::Protoss_Gateway);
  buildOrder->add(new SendScoutBuildOrderItem(BWAPI::UnitTypes::Protoss_Probe));
  buildOrder->add(BWAPI::UnitTypes::Protoss_Probe);
  buildOrder->add(BWAPI::UnitTypes::Protoss_Gateway);
  buildOrder->add(BWAPI::UnitTypes::Protoss_Zealot);
  buildOrder->add(BWAPI::UnitTypes::Protoss_Pylon);
  buildOrder->add(2, BWAPI::UnitTypes::Protoss_Zealot);
  buildOrder->add(BWAPI::UnitTypes::Protoss_Probe);
  buildOrder->add(new SwitchToAutomaticSupplyBuilding());
  buildOrder->add(10, BWAPI::UnitTypes::Protoss_Zealot);

  bhaalBot->buildOrderManager.executor.startBuildOrder(buildOrder);
}

void GatePushStrategy::onFrame()
{
  //TODO middle game strategy
}
