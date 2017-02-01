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
      if (UnitMemoryInfo* memoryInfo = unit->memoryInfo)
      {
        BWAPI::Broodwar->drawBoxMap(BWAPI::Position(unit->lastSeenPosition.x - unit->lastSeenUnitType.dimensionLeft(),
                                                    unit->lastSeenPosition.y - unit->lastSeenUnitType.dimensionUp()),
                                    BWAPI::Position(unit->lastSeenPosition.x + unit->lastSeenUnitType.dimensionLeft(),
                                                    unit->lastSeenPosition.y + unit->lastSeenUnitType.dimensionUp()),
                                    BWAPI::Colors::Grey);
        BWAPI::Broodwar->drawTextMap(BWAPI::Position(unit->lastSeenPosition.x - unit->lastSeenUnitType.dimensionLeft() + 5,
                                                     unit->lastSeenPosition.y - unit->lastSeenUnitType.dimensionUp()),
                                     "%s",
                                     Unit::shortenUnitName(unit->lastSeenUnitType.getName()).c_str());
      }
}

void DiscoveredMemory::onAdd(Unit* unit)
{
  if (!unit->getPlayer()->isEnemy)
    return;
  if (unit->getType().airWeapon().isValid())
    bhaalBot->dangerZones.addDanger(unit->getPosition(),
                                                     unit->getType().airWeapon().maxRange() / 32,
                                                     unit->getType().airWeapon().damageAmount());
}

void DiscoveredMemory::onRemove(Unit* unit)
{
  if (!unit->getPlayer()->isEnemy)
    return;
  if (unit->getType().airWeapon().isValid())
    bhaalBot->dangerZones.removeDanger(unit->lastSeenPosition,
                                       unit->lastSeenUnitType.airWeapon().maxRange() / 32,
                                       unit->lastSeenUnitType.airWeapon().damageAmount());
}

UnitMemoryInfo::UnitMemoryInfo(Unit* unit)
  : lastSeenTick(BWAPI::Broodwar->getFrameCount())
{}

void UnitMemoryInfo::update(Unit* unit)
{
  this->lastSeenTick = BWAPI::Broodwar->getFrameCount();
}
