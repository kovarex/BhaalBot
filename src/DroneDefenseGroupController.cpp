#include <Base.hpp>
#include <BaseHarvestingController.hpp>
#include <DroneDefenseGroupController.hpp>
#include <Group.hpp>
#include <Unit.hpp>

DroneDefenseGroupController::DroneDefenseGroupController(Group& owner, Base* base)
  : GroupController(owner)
  , base(base)
{
 
}

void DroneDefenseGroupController::onAdded(Unit* unit)
{
  this->goingToMineral.insert(unit);
}

void DroneDefenseGroupController::onRemoved(Unit* unit)
{
  this->stacked.erase(unit);
  this->goingToMineral.erase(unit);
}

void DroneDefenseGroupController::onFrame()
{
 if (this->defenseMineral == nullptr &&
     base->harvestingController &&
    !base->harvestingController->minerals.empty())
   this->defenseMineral = base->harvestingController->minerals[0]->mineral;
  if (this->defenseMineral)
    for (Unit* unit: this->owner.getUnits())
      unit->gather(this->defenseMineral);
  for (auto it = this->goingToMineral.begin(); it != this->goingToMineral.end();)
    if ((*it)->getDistance(this->defenseMineral) < 50)
    {
      this->stacked.insert(*it);
      it = this->goingToMineral.erase(it);
    }
    else
      ++it;
  for (Unit* fightingWorker: this->stacked)
  {
    if (fightingWorker->getGroundWeaponCooldown() > 0)
    {
      fightingWorker->gather(this->defenseMineral);
      continue;
    }
    if (fightingWorker->lastOrderGiven.type == Unit::Order::Type::Attack &&
        BWAPI::Broodwar->getFrameCount() - fightingWorker->frameOfLastOrder <= BWAPI::Broodwar->getLatencyFrames())
      continue;
      BWAPI::Unitset closeEnemies = BWAPI::Broodwar->getUnitsInRadius(fightingWorker->getPosition(), 40);
      for (BWAPI::Unit enemy: closeEnemies)
        if (BWAPI::Broodwar->self()->isEnemy(enemy->getPlayer()) &&
            enemy->canAttack(false))
          if (fightingWorker->getBWAPIUnit()->isInWeaponRange(enemy))
          {
            fightingWorker->frameOfLastOrder = BWAPI::Broodwar->getFrameCount();
            fightingWorker->lastOrderGiven = Unit::Order(Unit::Order::Type::Attack, enemy);
            fightingWorker->attack(enemy);
          }
  }
}
