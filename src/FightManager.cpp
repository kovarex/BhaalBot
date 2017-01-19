#include <FightManager.hpp>
#include <BhaalBot.hpp>
#include <DiscoveredMemory.hpp>
#include <Unit.hpp>

FightManager::FightManager(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

void FightManager::onUnitComplete(Unit* unit)
{
  if (unit->getPlayer() != BWAPI::Broodwar->self())
    return;
  if (unit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk)
  {
    for (MutaGroupManager& mutaGroup: this->mutaGroups)
      if (!mutaGroup.isFull())
      {
        mutaGroup.add(unit);
        return;
      }
    this->mutaGroups.push_back(MutaGroupManager());
    this->mutaGroups.back().add(unit);
  }
  else if (unit->canAttack() && !unit->canGather())
    this->otherUnits.push_back(unit);
}

void FightManager::onFrame()
{
  for (MutaGroupManager& mutaGroupManager: this->mutaGroups)
    mutaGroupManager.onFrame();
  if (this->mutaGroups.size() > 3)
  {
    //BWAPI::Position enemyMain = KovarexAIModule::instance->scoutingManager.enemyMainBase();
    if (!bhaalBot->discoveredMemory.units.empty())
    {
      BWAPI::Position target =  bhaalBot->discoveredMemory.units.begin()->second.position;
      for (MutaGroupManager& mutaGroupManager: this->mutaGroups)
        mutaGroupManager.target = target;
    }
  }
  else
  {
    if (!bhaalBot->harvestingManager.bases.empty())
    {
      BWAPI::Position target =  bhaalBot->harvestingManager.bases.back()->base->getPosition();
      for (MutaGroupManager& mutaGroupManager: this->mutaGroups)
      {
        mutaGroupManager.target = target;
        target.x += 50;
      }
    }
    else
    {
      BWAPI::Position target = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
      for (MutaGroupManager& mutaGroupManager: this->mutaGroups)
      {
        mutaGroupManager.target = target;
        target.x += 100;
      }
    }
  }
}

Unit* FightManager::freeUnit(BWAPI::UnitType unitType)
{
  for (uint32_t i = 0; i < this->otherUnits.size(); ++i)
    if (this->otherUnits[i]->getType() == unitType)
    {
      Unit* result = this->otherUnits[i];
      this->otherUnits.erase(this->otherUnits.begin() + i);
      return result;
    }
  return nullptr;
}
