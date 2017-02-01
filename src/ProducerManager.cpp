#include <ProducerManager.hpp>
#include <BhaalBot.hpp>
#include <Player.hpp>
#include <Unit.hpp>

ProducerManager::ProducerManager(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

void ProducerManager::onUnitComplete(Unit* unit)
{
  if (!this->isProducer(unit))
    return;
  if (unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery)
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
  return unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery ||
    unit->getType() == BWAPI::UnitTypes::Zerg_Lair ||
    unit->getType() == BWAPI::UnitTypes::Zerg_Hive;
}

Unit* ProducerManager::getBestProducer()
{
  Unit* bestProducer = nullptr;
  uint32_t bestProducerLarvaCount = 0;
  for (Unit* currentProducer: this->producers)
  {
    uint32_t currentProducerLarvaCount = int(currentProducer->getLarva().size()) -
             bhaalBot->larvaReservations.reservedLarvas(currentProducer);
    if (currentProducerLarvaCount <= 0)
      continue;
    if (bestProducer == nullptr)
    {
      bestProducer = currentProducer;
      bestProducerLarvaCount = currentProducerLarvaCount;
      continue;
    }
    if (bestProducerLarvaCount != currentProducerLarvaCount)
    {
      if (bestProducerLarvaCount > currentProducerLarvaCount)
        bestProducer = currentProducer;
      continue;
    }
  }
  return bestProducer;
}
