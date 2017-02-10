#include <BhaalBot.hpp>
#include <BuildOrder.hpp>
#include <BuildOrderManager.hpp>
#include <Strategy/HatchExpStrategy.hpp>
#include <StringUtil.hpp>
#include <Player.hpp>

bool HatchExpStrategy::initialized = HatchExpStrategy::initInstances();

HatchExpStrategy::HatchExpStrategy(int hatchCount)
  : Strategy(ssprintf("%d hatch exp", hatchCount), BWAPI::Races::Zerg, BWAPI::Races::None, false)
  , hatchCount(hatchCount)
{}

bool HatchExpStrategy::initInstances()
{
  for (uint32_t i = 2; i <= 4; ++i)
    new HatchExpStrategy(i);
  return false;
}

void HatchExpStrategy::onStart()
{
  this->buildOrder = new BuildOrder();
  buildOrder->add(new SendScoutBuildOrderItem(BWAPI::UnitTypes::Zerg_Overlord));
  buildOrder->add(new SwitchToAutomaticOverlordBuilding());
  bhaalBot->buildOrderManager.executor.startBuildOrder(buildOrder);
}

void HatchExpStrategy::onFrame()
{
  int hatchCount = bhaalBot->players.self->getUnitCountWithPlannedAndBuildOrderCombined(BWAPI::UnitTypes::Zerg_Hatchery);
  if (hatchCount >= this->hatchCount)
    return;
  int availableMinerals = BWAPI::Broodwar->self()->minerals() -
                          bhaalBot->costReservation.getReseved().minerals -
                          bhaalBot->buildOrderManager.executor.getPlannedCost().minerals;
  int droneCount = bhaalBot->players.self->getUnitCountWithPlannedAndBuildOrderCombined(BWAPI::UnitTypes::Zerg_Drone);
  if (availableMinerals >= 300 && droneCount > 5)
  {
    if (hatchCount < 2)
      this->buildOrder->add(BWAPI::UnitTypes::Zerg_Hatchery, BuildLocationType::ClosestExpansion);
    else
      this->buildOrder->add(BWAPI::UnitTypes::Zerg_Hatchery);
    return;
  }
  if (hatchCount < 2)
    if (droneCount >= 12)
      this->buildOrder->add(BWAPI::UnitTypes::Zerg_Hatchery);
    else
      this->buildOrder->add(BWAPI::UnitTypes::Zerg_Drone);
  else if (hatchCount < 3)
    if (droneCount >= 13)
      this->buildOrder->add(BWAPI::UnitTypes::Zerg_Hatchery);
    else
      this->buildOrder->add(BWAPI::UnitTypes::Zerg_Drone);
  else if (droneCount < 17 * 2)
    this->buildOrder->add(BWAPI::UnitTypes::Zerg_Drone);
}
