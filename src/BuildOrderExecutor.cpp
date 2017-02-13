#include <BuildOrderExecutor.hpp>
#include <BuildOrder.hpp>
#include <BhaalBot.hpp>
#include <StringUtil.hpp>
#include <BuildTaskInProgress.hpp>
#include <Unit.hpp>

BuildOrderExecutor::~BuildOrderExecutor()
{}

void BuildOrderExecutor::startBuildOrder(BuildOrder* buildOrder)
{
  this->currentBuildOrder = buildOrder;
  this->stepToDo = 0;
}

void BuildOrderExecutor::update()
{
  if (this->currentBuildOrder == nullptr)
    return;
  for (uint32_t i = this->stepToDo; i < stepToDo + 5 && i < this->currentBuildOrder->items.size(); ++i)
    BWAPI::Broodwar->drawText(BWAPI::CoordinateType::Screen,
                              0, (i - this->stepToDo) * 20,
                              "%d) %s",
                              i + 1, this->currentBuildOrder->items[i]->str().c_str());

  if (this->automaticSupplyBuilding)
  {
    int32_t reserve = BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed();
    reserve +=  bhaalBot->morphingUnits.getPlannedCount(BWAPI::UnitTypes::Zerg_Overlord) * 16;
    reserve += bhaalBot->buildTasks.plannedCount(BWAPI::UnitTypes::Terran_Supply_Depot) * 16;
    reserve += bhaalBot->buildTasks.plannedCount(BWAPI::UnitTypes::Protoss_Pylon) * 16;
    if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
    {
      if (reserve <= int32_t(bhaalBot->produceManager.producers.size()) * 3)
      {
        this->train(BWAPI::UnitTypes::Zerg_Overlord);
        return;
      }
    }
    else if (reserve <= int32_t(bhaalBot->produceManager.producers.size()) * 4)
    {
      bhaalBot->buildTasks.buildTasksInProgress.push_back(new BuildTaskInProgress(BuildLocationType::Auto,
                                                                                  BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss ?
                                                                                  BWAPI::UnitTypes::Protoss_Pylon : BWAPI::UnitTypes::Terran_Supply_Depot));
      return;
    }
  }
  if (this->currentBuildOrder->items.size() <= this->stepToDo)
    return;
  if (this->currentBuildOrder->items[this->stepToDo]->execute())
    ++stepToDo;
}

bool BuildOrderExecutor::train(BWAPI::UnitType unitType)
{
  Unit* producer =  bhaalBot->produceManager.getBestProducer(unitType);
  if (producer)
    if (producer->getType().getRace() == BWAPI::Races::Zerg)
      return bhaalBot->larvaReservations.tryToTrain(producer, unitType);
    else
      return bhaalBot->productionQueueReservations.tryToTrain(producer, unitType);
  return false;
}

bool BuildOrderExecutor::isFinished()
{
  return this->currentBuildOrder->items.size() <= this->stepToDo;
}

int BuildOrderExecutor::getPlannedType(BWAPI::UnitType unitType)
{
  int result = 0;
  for (uint32_t i = this->stepToDo; i < this->currentBuildOrder->items.size(); ++i)
    result += this->currentBuildOrder->items[i]->getPlannedType(unitType);
  return result;
}

Cost BuildOrderExecutor::getPlannedCost()
{
  Cost result;
  for (uint32_t i = this->stepToDo; i < this->currentBuildOrder->items.size(); ++i)
    result += this->currentBuildOrder->items[i]->getCost();
  return result;
}

std::string BuildTaskInProgress::str() const
{
  return ssprintf("Build %s", this->unitType.getName().c_str());
}
