#include <BhaalBot.hpp>
#include <BuildOrder.hpp>
#include <BuildOrderManager.hpp>
#include <Strategy/LingRushStrategy.hpp>
#include <Strategy/Strategies.hpp>
#include <StringUtil.hpp>

bool LingRushStrategy::initialized = LingRushStrategy::initInstances();

LingRushStrategy::LingRushStrategy(int dronesWhenPool)
  : Strategy(ssprintf("%d pool", dronesWhenPool), BWAPI::Races::Zerg, BWAPI::Races::None, false)
  , dronesWhenPool(dronesWhenPool)
{}

bool LingRushStrategy::initInstances()
{
  for (uint32_t i = 4; i <= 6; ++i)
    new LingRushStrategy(i);
  return true;
}

void LingRushStrategy::onStart()
{
  BuildOrder* buildOrder = new BuildOrder();
  buildOrder->add(new SendScoutBuildOrderItem(BWAPI::UnitTypes::Zerg_Overlord));
  buildOrder->add(this->dronesWhenPool - 4, BWAPI::UnitTypes::Zerg_Drone);
  buildOrder->add(BWAPI::UnitTypes::Zerg_Spawning_Pool);
  buildOrder->add(1, BWAPI::UnitTypes::Zerg_Drone);
  buildOrder->add(new SendScoutBuildOrderItem(BWAPI::UnitTypes::Zerg_Drone));
  buildOrder->add(3, BWAPI::UnitTypes::Zerg_Zergling);
  buildOrder->add(new SwitchToAutomaticSupplyBuilding());
  buildOrder->add(5, BWAPI::UnitTypes::Zerg_Zergling);

  bhaalBot->buildOrderManager.executor.startBuildOrder(buildOrder);
}

void LingRushStrategy::onFrame()
{
  if (bhaalBot->buildOrderManager.executor.isFinished() &&
      this->switchOrder == nullptr)
    this->switchOrder = Strategies::instance().strategyByName("3 hatch exp");
}
