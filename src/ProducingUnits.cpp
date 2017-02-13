#include <BhaalBot.hpp>
#include <ProducingUnits.hpp>
#include <Unit.hpp>

ProducingUnits::ProducingUnits(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

void ProducingUnits::onUnitCreate(Unit* unit)
{
  if (unit->getPlayer() != bhaalBot->players.self)
    return;
  if (unit->getType().isBuilding())
    return;
  if (unit->getRemainingBuildTime() == 0)
    return;
  if (Unit* producer = this->deduceProducer(unit))
    this->unitsBeingProduced[unit] = UnitToBeProducedData(producer);
}

void ProducingUnits::onUnitComplete(Unit* unit)
{
  this->unitsBeingProduced.erase(unit);
}

void ProducingUnits::onUnitDestroy(Unit* unit)
{
  this->unitsBeingProduced.erase(unit);
}

Unit* ProducingUnits::deduceProducer(Unit* unit)
{
  for (Unit* producer: bhaalBot->produceManager.producers)
    if (producer->getType().buildsWhat().contains(unit->getType()) &&
        unit->getPosition() == producer->getPosition())
      return producer;
  return nullptr;
}

Unit* ProducingUnits::getProducingUnit(Unit* producer)
{
  for (auto& item: this->unitsBeingProduced)
    if (item.second.producer == producer)
      return item.first;
  return nullptr;
}
