#include <DiscoveredMemory.hpp>
#include <BhaalBot.hpp>
#include <UnitTarget.hpp>
#include <Unit.hpp>
#include <Units.hpp>
#include <Log.hpp>
#include <Player.hpp>

DiscoveredMemory::DiscoveredMemory(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

void DiscoveredMemory::onFrame()
{
  for (Player* enemyPlayer: bhaalBot->players.enemies)
    for (Unit* unit: enemyPlayer->units)
    {
      if (unit->isVisible())
        this->addUnit(unit);

      if (UnitMemoryInfo* memoryInfo = unit->memoryInfo)
      {
        BWAPI::Broodwar->drawBoxMap(BWAPI::Position(memoryInfo->position.x - unit->lastSeenUnitType.dimensionLeft(),
                                                    memoryInfo->position.y - unit->lastSeenUnitType.dimensionUp()),
                                    BWAPI::Position(memoryInfo->position.x + unit->lastSeenUnitType.dimensionLeft(),
                                                    memoryInfo->position.y + unit->lastSeenUnitType.dimensionUp()),
                                    BWAPI::Colors::Grey);
        BWAPI::Broodwar->drawTextMap(memoryInfo->position, "%s", unit->lastSeenUnitType.getName().c_str());
      }
    }
}

void DiscoveredMemory::onUnitComplete(Unit* unit)
{
  if (unit->getPlayer()->isEnemy)
    return;
  this->addUnit(unit);
}

void DiscoveredMemory::onUnitDestroy(Unit* unit)
{
  if (unit->getPlayer()->isEnemy)
    return;
  if (unit->memoryInfo != nullptr)
    this->onRemove(unit);
}

void DiscoveredMemory::addUnit(Unit* unit)
{
  if (unit->memoryInfo)
  {
    this->onRemove(unit);
    unit->memoryInfo->update(unit);
  }
  else
    unit->memoryInfo = new UnitMemoryInfo(unit);
  this->onAdd(unit);
}

void DiscoveredMemory::onAdd(Unit* unit)
{
  if (unit->getType().airWeapon().isValid())
    bhaalBot->dangerZones.addDanger(unit->getPosition(),
                                                     unit->getType().airWeapon().maxRange() / 32,
                                                     unit->getType().airWeapon().damageAmount());
}

void DiscoveredMemory::onRemove(Unit* unit)
{
  if (unit->getType().airWeapon().isValid())
    bhaalBot->dangerZones.removeDanger(unit->memoryInfo->position,
                                       unit->lastSeenUnitType.airWeapon().maxRange() / 32,
                                       unit->lastSeenUnitType.airWeapon().damageAmount());
}

UnitMemoryInfo::UnitMemoryInfo(Unit* unit)
  : position(unit->getPosition())
  , lastSeenTick(BWAPI::Broodwar->getFrameCount())
{}

void UnitMemoryInfo::update(Unit* unit)
{
  this->position = unit->getPosition();
  this->lastSeenTick = unit->getType();
  this->lastSeenTick = BWAPI::Broodwar->getFrameCount();
}
