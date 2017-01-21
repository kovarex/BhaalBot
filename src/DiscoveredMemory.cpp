#include <DiscoveredMemory.hpp>
#include <BhaalBot.hpp>
#include <EnemyUnitTarget.hpp>

DiscoveredMemory::DiscoveredMemory(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

DiscoveredMemory::~DiscoveredMemory()
{
  for (auto& item: this->units)
    delete item.second;
}

void DiscoveredMemory::onFrame()
{
  for (BWAPI::Unit unit: BWAPI::Broodwar->getAllUnits())
    if (unit->isVisible() &&
        BWAPI::Broodwar->self()->isEnemy(unit->getPlayer()))
      this->addUnit(unit);
  for (auto it = this->units.begin(); it != this->units.end();)
  {
    auto& item = *it;
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(item.second->position.x - item.second->unitType.dimensionLeft(),
                                                item.second->position.y - item.second->unitType.dimensionUp()),
                                BWAPI::Position(item.second->position.x + item.second->unitType.dimensionLeft(),
                                                item.second->position.y + item.second->unitType.dimensionUp()),
                                BWAPI::Colors::Grey);
    BWAPI::Broodwar->drawTextMap(item.second->position, "%s", item.second->unitType.getName().c_str());
    if (!item.second->unitType.isBuilding() && BWAPI::Broodwar->getFrameCount() - item.second->lastSeenTick > 24 * 10)
    {
      this->onRemove(it);
      it = this->units.erase(it);
    }
    else
      ++it;
  }
}

void DiscoveredMemory::onForeignUnitComplete(BWAPI::Unit unit)
{
  if (unit->getPlayer()->isNeutral() || !BWAPI::Broodwar->self()->isEnemy(unit->getPlayer()))
    return;
  this->addUnit(unit);
}

void DiscoveredMemory::onForeignUnitDestroy(BWAPI::Unit unit)
{
  if (unit->getPlayer()->isNeutral() || !BWAPI::Broodwar->self()->isEnemy(unit->getPlayer()))
    return;
   auto position = this->units.find(unit);
  if (position != this->units.end())
    this->onRemove(position);
  delete position->second;
  this->units.erase(position);
}

void DiscoveredMemory::addUnit(BWAPI::Unit unit)
{
  auto position = this->units.find(unit);
  if (position != this->units.end())
  {
    this->onRemove(position);
    position->second->update(unit);
    
  }
  else
    this->units[unit] = new UnitMemoryInfo(unit);
  this->onAdd(unit);
}

void DiscoveredMemory::onAdd(BWAPI::Unit unit)
{
  if (unit->getType().airWeapon().isValid())
    bhaalBot->dangerZones.addDanger(unit->getPosition(),
                                                     unit->getType().airWeapon().maxRange() / 32,
                                                     unit->getType().airWeapon().damageAmount());
}

void DiscoveredMemory::onRemove(std::map<BWAPI::Unit, UnitMemoryInfo*>::iterator unit)
{
  if (unit->second->unitType.airWeapon().isValid())
    bhaalBot->dangerZones.removeDanger(unit->second->position,
                                       unit->second->unitType.airWeapon().maxRange() / 32,
                                       unit->second->unitType.airWeapon().damageAmount());
}

UnitMemoryInfo* DiscoveredMemory::getUnit(BWAPI::Unit unit)
{
  auto position = this->units.find(unit);
  if (position != this->units.end())
    return position->second;
  return nullptr;
}

UnitMemoryInfo::UnitMemoryInfo(BWAPI::Unit unit)
  : unit(unit)
  , position(unit->getPosition())
  , unitType(unit->getType())
  , lastSeenTick(BWAPI::Broodwar->getFrameCount())
{}

UnitMemoryInfo::~UnitMemoryInfo()
{
  for (EnemyUnitTarget* target: this->targetingMe)
    target->data = nullptr;
}

void UnitMemoryInfo::update(BWAPI::Unit unit)
{
  this->position = unit->getPosition();
  this->unitType = unit->getType();
  this->lastSeenTick = BWAPI::Broodwar->getFrameCount();
}

void UnitMemoryInfo::addTarget(EnemyUnitTarget* target)
{
  this->targetingMe.push_back(target);
}

void UnitMemoryInfo::removeTarget(EnemyUnitTarget* target)
{
  for (auto it = this->targetingMe.begin(); it != this->targetingMe.end(); ++it)
    if (*it == target)
    {
      this->targetingMe.erase(it);
      return;
    }
  throw std::runtime_error("Trying to remove target that is not here.");
}
