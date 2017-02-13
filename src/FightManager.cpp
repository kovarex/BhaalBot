#include <FightManager.hpp>
#include <BhaalBot.hpp>
#include <DiscoveredMemory.hpp>
#include <Unit.hpp>
#include <TaskForceController.hpp>
#include <Player.hpp>

FightManager::FightManager(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{
  this->taskForces.push_back(this->attack = new TaskForce());
  this->attack->assignTaskController(new AttackTaskForceController(*this->attack));
}

FightManager::~FightManager()
{
  for (TaskForce* taskForce: this->taskForces)
    delete taskForce;
}

void FightManager::onUnitComplete(Unit* unit)
{
  if (unit->getPlayer() == nullptr ||
      !unit->getPlayer()->myself)
    return;
  if (unit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk)
    this->attack->addUnit(unit);
  else if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling)
    this->attack->addUnit(unit);
  //else if (unit->canAttack() && !unit->canGather())
  //  this->baseDefend->addUnit(unit);
}

void FightManager::onFrame()
{
  for (TaskForce* taskForce: this->taskForces)
    taskForce->onFrame();
}

Unit* FightManager::getUnit(BWAPI::UnitType unitType)
{
  for (TaskForce* taskForce: this->taskForces)
    if (Unit* unit = taskForce->getUnit(unitType))
      return unit;
  return nullptr;
}
