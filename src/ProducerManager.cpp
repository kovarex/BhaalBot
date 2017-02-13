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
  Unit* bestProducer = nullptr;
  int bestFreeQueueSlots = 0; // Or larva count
  for (Unit* currentProducer: this->producers)
    if (currentProducer->getType().getRace() == BWAPI::Races::Zerg ||
        currentProducer->canTrain(unitType))
    {
      int currentFreeQueueSlots = 0;
      if (currentProducer->getType().getRace() == BWAPI::Races::Zerg)
        currentFreeQueueSlots = int(currentProducer->getLarva().size()) -
                 bhaalBot->larvaReservations.reservedLarvas(currentProducer);
      else
        currentFreeQueueSlots = 2 - currentProducer->getBWAPIUnit()->getTrainingQueue().size();
      if (currentFreeQueueSlots <= 0)
        continue;
      if (bestProducer == nullptr)
      {
        bestProducer = currentProducer;
        bestFreeQueueSlots = currentFreeQueueSlots;
        continue;
      }
      if (currentFreeQueueSlots > bestFreeQueueSlots)
      {
        bestProducer = currentProducer;
        bestFreeQueueSlots = currentFreeQueueSlots;
      }
    }
  return bestProducer;
}
