#include <BWAPIUtil.hpp>
#include <BhaalBot.hpp>
#include <Base.hpp>
#include <BaseHarvestingController.hpp>
#include <DroneDefenseGroupController.hpp>
#include <Group.hpp>
#include <Unit.hpp>
#include <Vector.hpp>

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

bool DroneDefenseGroupController::isOccupiedByMineral(BWAPI::Position position)
{
  BWAPI::Unitset neighbours = BWAPI::Broodwar->getUnitsInRadius(position, 0);
  for (BWAPI::Unit neighbour: neighbours)
    if (neighbour->getType().isMineralField())
      return true;
  return false;
}

BWAPI::Unit DroneDefenseGroupController::getBestMineralForDefense(bool drawDebugInfo)
{
  BWAPI::Position baseCenter = this->base->getCenter();
  BWAPI::Unit bestMineral = nullptr;
  uint32_t bestMineralSidesBlocked = 0;

  for (BaseHarvestingController::Mineral* mineral: this->base->harvestingController->minerals)
  {
    BWAPI::Position rightTilePosition = mineral->mineral->getPosition();
    rightTilePosition.x += 16;
    BWAPI::TilePosition rightTile(rightTilePosition);
    BWAPI::TilePosition rightTileNextToMineral(rightTilePosition);
    rightTileNextToMineral.x++;
    BWAPI::TilePosition leftTileNextToMineral(rightTilePosition);
    leftTileNextToMineral.x -= 2;

    BWAPI::Position leftNextToMineral(BWAPIUtil::tileCenter(leftTileNextToMineral));
    BWAPI::Position rightNextToMineral(BWAPIUtil::tileCenter(rightTileNextToMineral));

    BWAPI::Position sideOfMineralCloserToBase = leftNextToMineral.getDistance(baseCenter) < rightNextToMineral.getDistance(baseCenter) ?
      leftNextToMineral : rightNextToMineral;
   
    if (drawDebugInfo)
      BWAPI::Broodwar->drawCircleMap(sideOfMineralCloserToBase, 5, BWAPI::Colors::Red);

    uint32_t sidesBlocked = 0;
    for (const Vector& vector: Vector::basicOneTileDirections)
    {
      BWAPI::Position positionToCheck = sideOfMineralCloserToBase;
      vector.addTo(&positionToCheck);
      if (this->isOccupiedByMineral(positionToCheck))
      {
        sidesBlocked += 1;
        BWAPI::Position inBetween((sideOfMineralCloserToBase.x + positionToCheck.x) / 2,
                                  (sideOfMineralCloserToBase.y + positionToCheck.y) / 2);
        BWAPI::Broodwar->drawLineMap(sideOfMineralCloserToBase, inBetween, BWAPI::Colors::Red);
        BWAPI::Broodwar->drawBoxMap(BWAPI::Position(inBetween.x - 3, inBetween.y - 3),
                                    BWAPI::Position(inBetween.x + 3, inBetween.y + 3), BWAPI::Colors::Red);
      }
    }
    if (bestMineral == nullptr ||
        bestMineralSidesBlocked < sidesBlocked)
    {
      bestMineral = mineral->mineral;
      bestMineralSidesBlocked = sidesBlocked;
    }
  }
  return bestMineral;
}

void DroneDefenseGroupController::onFrame()
{
  if (this->defenseMineral == nullptr)
    this->defenseMineral = this->getBestMineralForDefense();

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
    /*if (fightingWorker->getGroundWeaponCooldown() > 0)
    {
      fightingWorker->gather(this->defenseMineral);
      continue;
    }*/
    if (fightingWorker->lastOrderGiven.type == Unit::Order::Type::Attack &&
        BWAPI::Broodwar->getFrameCount() - fightingWorker->frameOfLastOrder <= BWAPI::Broodwar->getLatencyFrames())
      continue;
    if (fightingWorker->isAttackFrame())
      continue;
    BWAPI::Unitset closeEnemies = BWAPI::Broodwar->getUnitsInRadius(fightingWorker->getPosition(), 40);
    BWAPI::Unit bestEnemyToAttack = nullptr;;
    for (BWAPI::Unit enemy: closeEnemies)
      if (BWAPI::Broodwar->self()->isEnemy(enemy->getPlayer()) &&
          enemy->canAttack(false) &&
          fightingWorker->getBWAPIUnit()->isInWeaponRange(enemy) &&
          (bestEnemyToAttack == nullptr ||
           bestEnemyToAttack->getHitPoints() > enemy->getHitPoints()))
        bestEnemyToAttack = enemy;
    if (bestEnemyToAttack)
    {
      fightingWorker->frameOfLastOrder = BWAPI::Broodwar->getFrameCount();
      fightingWorker->lastOrderGiven = Unit::Order(Unit::Order::Type::Attack, bhaalBot->units.findOrThrow(bestEnemyToAttack));
      fightingWorker->attack(bestEnemyToAttack);
    }
    else
      if (fightingWorker->getDistance(this->defenseMineral) > 80)
        fightingWorker->gather(this->defenseMineral);
  }
}
