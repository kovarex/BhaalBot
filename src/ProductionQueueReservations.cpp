#include <BhaalBot.hpp>
#include <ProductionQueueReservations.hpp>
#include <StringUtil.hpp>
#include <Unit.hpp>

ProductionQueueReservations::ProductionOrderInProgress::ProductionOrderInProgress(Unit* producer, BWAPI::UnitType targetUnit)
  : CostReservationItem(Cost(targetUnit.mineralPrice(), targetUnit.gasPrice()))
  , producer(producer)
  , targetUnit(targetUnit)
  , tickOfLastOrder(BWAPI::Broodwar->getFrameCount())
{}

std::string ProductionQueueReservations::ProductionOrderInProgress::str() const
{
  return ssprintf("Producing %s", Unit::shortenUnitName(this->targetUnit.getName()).c_str());
}

ProductionQueueReservations::ProductionQueueReservations(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

ProductionQueueReservations::~ProductionQueueReservations()
{
  for (auto& item: this->ordersInProgress)
    for (ProductionOrderInProgress* productionInProgress: item.second)
      delete productionInProgress;
}

bool ProductionQueueReservations::tryToTrain(Unit* producer, BWAPI::UnitType targetUnit)
{
  this->ordersInProgress[producer].push_back(new ProductionOrderInProgress(producer, targetUnit));
  producer->train(targetUnit);
  return true;
}

void ProductionQueueReservations::onUnitCreate(Unit* unit)
{
  if (unit->getPlayer() != bhaalBot->players.self)
    return;
  if (unit->getType().isBuilding())
    return;
  if (unit->getRemainingBuildTime() == 0)
    return;
  if (Unit* producer = bhaalBot->producingUnits.deduceProducer(unit))
  {
    auto position = this->ordersInProgress.find(producer);
    if (position == this->ordersInProgress.end())
      return;
    for (uint32_t i = 0; i < position->second.size(); ++i)
    {
      if (position->second[i]->targetUnit == unit->getType())
      {
        delete position->second[i];
        position->second.erase(position->second.begin() + i);
        return;
      }
    }
    LOG_WARNING("Didn't find producer of %s at [%d,%d]", unit->getType().getName().c_str(), unit->getPosition().x, unit->getPosition().y);
  }
}

int ProductionQueueReservations::plannedCount(BWAPI::UnitType unitType)
{
  int result = 0;
  for (auto& item: this->ordersInProgress)
    for (ProductionOrderInProgress* productionInProgress: item.second)
      if (productionInProgress->targetUnit == unitType)
        ++result;
  return result;
}

void ProductionQueueReservations::onFrame()
{
  for (auto& item: this->ordersInProgress)
    for (ProductionOrderInProgress* productionInProgress: item.second)
      if (BWAPI::Broodwar->getFrameCount() - productionInProgress->tickOfLastOrder > BWAPI::Broodwar->getLatencyFrames() + 4)
      {
        productionInProgress->producer->train(productionInProgress->targetUnit);
        productionInProgress->tickOfLastOrder = BWAPI::Broodwar->getFrameCount();
      }
}
