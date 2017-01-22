#include <FightManager.hpp>
#include <BhaalBot.hpp>
#include <DiscoveredMemory.hpp>
#include <Unit.hpp>
#include <TaskForceController.hpp>

FightManager::FightManager(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{
  this->taskForces.push_back(this->baseDefend = new TaskForce());
  this->baseDefend->assignTaskController(new DefendTaskForceController(*this->baseDefend));

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
  if (unit->getPlayer() != BWAPI::Broodwar->self())
    return;
  if (unit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk)
    this->attack->addUnit(unit);
  else if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling)
    this->attack->addUnit(unit);
  else if (unit->canAttack() && !unit->canGather())
    this->baseDefend->addUnit(unit);
}

void FightManager::onFrame()
{
  for (TaskForce* taskForce: this->taskForces)
    taskForce->onFrame();
}

Unit* FightManager::getUnit(BWAPI::UnitType unitType)
{
  return this->baseDefend->getUnit(unitType);
}
