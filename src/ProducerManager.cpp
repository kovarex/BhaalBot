#include <ProducerManager.hpp>
#include <BhaalBot.hpp>
#include <Player.hpp>
#include <Unit.hpp>

ProducerManager::ProducerManager(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

void ProducerManager::onUnitComplete(Unit* unit)
{
  if (unit->getPlayer() == bhaalBot->players.self &&
      this->isProducer(unit))
    this->producers.push_back(unit);
}

void ProducerManager::onUnitDestroy(Unit* unit)
{
  if (!this->isProducer(unit))
    return;
  for (auto it = this->producers.begin(); it != this->producers.end(); ++it)
    if (*it == unit)
    {
      this->producers.erase(it);
      return;
    }
}

bool ProducerManager::isProducer(Unit* unit)
{
  if (unit->getType().getRace() == BWAPI::Races::Zerg)
    return unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery ||
      unit->getType() == BWAPI::UnitTypes::Zerg_Lair ||
      unit->getType() == BWAPI::UnitTypes::Zerg_Hive;
  return unit->getType().canProduce();
}

Unit* ProducerManager::getBestProducer(BWAPI::UnitType unitType)
{
  if (unitType.getRace() == BWAPI::Races::Zerg)
    return this->getBestZergProducer();

  for (Unit* currentProducer: this->producers)
    if (currentProducer->canTrain(unitType))
    {
      int currentQueueSize = int(currentProducer->getBWAPIUnit()->getTrainingQueue().size());
      int reservedProductions =  int(bhaalBot->productionQueueReservations.ordersInProgress[currentProducer].size());
      if (currentQueueSize + reservedProductions > 1 ||
          reservedProductions != 0)
        continue;
      if (currentQueueSize == 1)
      {
        Unit* producingUnit = bhaalBot->producingUnits.getProducingUnit(currentProducer);
        if (producingUnit == nullptr ||
            producingUnit->getRemainingBuildTime() > BWAPI::Broodwar->getLatencyFrames())
          continue;
      }
      return currentProducer;
    }
  return nullptr;
}

Unit* ProducerManager::getBestZergProducer()
{
  Unit* bestProducer = nullptr;
  int bestFreeQueueSlots = 0; // Or larva count
  for (Unit* currentProducer: this->producers)
  {
    int currentFreeLarva = int(currentProducer->getLarva().size()) -
                bhaalBot->larvaReservations.reservedLarvas(currentProducer);
    if (currentFreeLarva <= 0)
      continue;
    if (bestProducer == nullptr)
    {
      bestProducer = currentProducer;
      bestFreeQueueSlots = currentFreeLarva;
      continue;
    }
    if (currentFreeLarva > bestFreeQueueSlots)
    {
      bestProducer = currentProducer;
      bestFreeQueueSlots = currentFreeLarva;
    }
  }
  return bestProducer;
}
