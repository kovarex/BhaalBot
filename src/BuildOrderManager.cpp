#include <BuildOrderManager.hpp>
#include <BhaalBot.hpp>
#include <BWEM/bwem.h>
#include <BuildOrder.hpp>

BuildOrderManager::BuildOrderManager(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{
  {
    BuildOrder* pool9 = new BuildOrder();
    pool9->add(5, BWAPI::UnitTypes::Zerg_Drone);
    pool9->add(BWAPI::UnitTypes::Zerg_Spawning_Pool);
    pool9->add(BWAPI::UnitTypes::Zerg_Drone);
    pool9->add(BWAPI::UnitTypes::Zerg_Overlord);
    pool9->add(BWAPI::UnitTypes::Zerg_Drone);
    pool9->add(6, BWAPI::UnitTypes::Zerg_Zergling);
    this->buildOrders.push_back(pool9);
  }
  {
    BuildOrder*  pool11Exp = new BuildOrder();
    pool11Exp->add(5, BWAPI::UnitTypes::Zerg_Drone);
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Overlord);
    pool11Exp->add(2, BWAPI::UnitTypes::Zerg_Drone);
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Spawning_Pool);
    pool11Exp->add(3, BWAPI::UnitTypes::Zerg_Drone);
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Hatchery, BuildLocationType::ClosestExpansion);
    pool11Exp->add(2, BWAPI::UnitTypes::Zerg_Zergling);
    for (uint32_t i = 0; i < 2; ++i)
      pool11Exp->add(new SendScoutBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling));
    pool11Exp->add(2, BWAPI::UnitTypes::Zerg_Zergling);
    for (uint32_t i = 0; i < 2; ++i)
      pool11Exp->add(new SendScoutBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling));
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Hatchery);
    pool11Exp->add(6, BWAPI::UnitTypes::Zerg_Drone);
    pool11Exp->add(2, BWAPI::UnitTypes::Zerg_Extractor);
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Overlord);
    pool11Exp->add(new SwitchToAutomaticOverlordBuilding());
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Creep_Colony, BuildLocationType::NearFirstExpansion);
    pool11Exp->add(2, BWAPI::UnitTypes::Zerg_Drone);
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Creep_Colony, BuildLocationType::NearFirstExpansion);
    pool11Exp->add(2, BWAPI::UnitTypes::Zerg_Drone);
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Creep_Colony, BuildLocationType::NearFirstExpansion);
    pool11Exp->add(2, BWAPI::UnitTypes::Zerg_Drone);
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Creep_Colony, BuildLocationType::NearFirstExpansion);
    pool11Exp->add(2, BWAPI::UnitTypes::Zerg_Drone);
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Creep_Colony, BuildLocationType::NearFirstExpansion);
    pool11Exp->add(5, BWAPI::UnitTypes::Zerg_Sunken_Colony);
    pool11Exp->add(5, BWAPI::UnitTypes::Zerg_Drone);
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Lair);
    pool11Exp->add(11, BWAPI::UnitTypes::Zerg_Drone);
    pool11Exp->add(BWAPI::UnitTypes::Zerg_Spire);
    pool11Exp->add(6, BWAPI::UnitTypes::Zerg_Zergling);
    for (uint32_t i = 0; i < 6; ++i)
      pool11Exp->add(new SendScoutBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling));
    pool11Exp->add(3, BWAPI::UnitTypes::Zerg_Overlord);
    for (uint32_t i = 0; i < 50; ++i)
    {
      pool11Exp->add(5, BWAPI::UnitTypes::Zerg_Mutalisk);
      pool11Exp->add(1, BWAPI::UnitTypes::Zerg_Zergling);
      for (uint32_t i = 0; i < 2; ++i)
        pool11Exp->add(new SendScoutBuildOrderItem(BWAPI::UnitTypes::Zerg_Zergling));
    }
    this->buildOrders.push_back(pool11Exp);
  }
  this->executor.startBuildOrder(this->buildOrders.back());
}

BuildOrderManager::~BuildOrderManager()
{
  for (BuildOrder* buildOrder: buildOrders)
    delete buildOrder;
}

void BuildOrderManager::onFrame()
{
  this->executor.update();
}

void BuildOrderManager::onUnitComplete(BWAPI::Unit)
{}

BuildOrder::~BuildOrder()
{
  for (BuildOrderItem* item: this->items)
    delete item;
}

void BuildOrder::add(uint32_t count, BWAPI::UnitType unit)
{
  for (uint32_t i = 0; i < count; ++i)
    this->add(unit);
}

void BuildOrder::add(BWAPI::UnitType unit, BuildLocationType location)
{
  this->items.push_back(new BuildBuildOrderItem(unit, location));
}

void BuildOrder::add(BuildOrderItem* item)
{
  this->items.push_back(item);
}
