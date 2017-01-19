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
  for (uint32_t i = this->stepToDo; i < stepToDo + 5 && i < this->currentBuildOrder->items.size(); ++i)
    BWAPI::Broodwar->drawText(BWAPI::CoordinateType::Screen,
                              0, (i - this->stepToDo) * 20,
                              "%d) %s",
                              i + 1, this->currentBuildOrder->items[i]->str().c_str());

  if (this->automaticOverlordBuilding)
  {
    int32_t reserve = BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed();
    reserve +=  bhaalBot->morphingUnits.getMorphingCount(BWAPI::UnitTypes::Zerg_Overlord) * 8;
    if (reserve <= int32_t( bhaalBot->produceManager.producers.size()))
    {
      this->train(BWAPI::UnitTypes::Zerg_Overlord);
      return;
    }
  }
  if (this->currentBuildOrder == nullptr)
    return;
  if (this->currentBuildOrder->items.size() <= this->stepToDo)
    return;
  if (this->currentBuildOrder->items[this->stepToDo]->execute())
  {
    ++stepToDo;
    if (this->currentBuildOrder->items.size() == this->stepToDo)
    BWAPI::Broodwar->sendText("Finished build order");
  }
}

bool BuildOrderExecutor::train(BWAPI::UnitType unitType)
{
  Unit* producer =  bhaalBot->produceManager.getBestProducer();
  if (producer && producer->canTrain(unitType))
    return  bhaalBot->larvaReservations.tryToTrain(producer, unitType);
  return false;
}

std::string BuildTaskInProgress::str() const
{
  return ssprintf("Build %s", this->unitType.getName().c_str());
}
