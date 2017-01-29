#include <ScoutingManager.hpp>
#include <BhaalBot.hpp>
#include <Unit.hpp>
#include <Base.hpp>
#include <Log.hpp>

ScoutingManager::ScoutingManager(ModuleContainer& moduleContainer)\
  : Module(moduleContainer)
{}

ScoutingManager::~ScoutingManager()
{
  while (!this->scoutTasks.empty())
    this->scoutTasks.front().scout->assign(nullptr);
  while (!this->baseCheckTasks.empty())
    this->baseCheckTasks.front().scout->assign(nullptr);
}

void ScoutingManager::assignGroundScout(Unit* unit)
{
  this->unassignedGroundScouters.push_back(unit);
  unit->assign(new ScoutTaskAssignment());
}

Unit* ScoutingManager::getScoutCandidate(BWAPI::UnitType unitType)
{
  for (auto& item: bhaalBot->units.getUnits())
    if (item.second->getType() == unitType &&
        item.second->canMove() &&
        (item.second->getAssignment() == nullptr ||
         item.second->getAssignment()->getPriority() == Assignment::Priority::Low))
      return item.second;
  return nullptr;
}


void ScoutingManager::onFrame()
{
  for (auto it = this->ordersToScout.begin(); it != this->ordersToScout.end();)
    if (Unit* unit = this->getScoutCandidate(it->first))
    {
      unit->assign(nullptr);
      bhaalBot->scoutingManager.assignGroundScout(unit);
      it->second--;
      if (it->second == 0)
        it = this->ordersToScout.erase(it);
    }
    else
      ++it;

  while (!this->unassignedGroundScouters.empty())
  {
    Base* base = this->baseToScout();
    if (base == nullptr)
      break;
    this->scoutTasks.push_back(DiscoverScoutingLocationsScoutTask(base, this->unassignedGroundScouters.back()));
    this->unassignedGroundScouters.pop_back();
  }
  for (uint32_t i = 0; i < this->scoutTasks.size();)
  {
    DiscoverScoutingLocationsScoutTask& task = this->scoutTasks[i];
    task.onFrame();
    if (task.finished)
    {
      Unit* unit = task.scout;
      unit->assign(nullptr);
      bhaalBot->moduleContainer.onUnitIdle(unit);
    }
    else
    {
      BWAPI::Broodwar->drawTextMap(task.scout->getPosition(), "Scout of starting location");
      BWAPI::Broodwar->drawLineMap(task.scout->getPosition(), task.target->getCenter(), BWAPI::Colors::White);
      ++i;
    }
  }

  for (auto it = this->baseCheckTasks.begin(); it != this->baseCheckTasks.end();)
  {
    it->onFrame();
    if (it->finished)
    {
      Unit* unit = it->scout;
      ++it;
      unit->assign(nullptr);
      bhaalBot->moduleContainer.onUnitIdle(unit);
    }
    else
    {
      BWAPI::Broodwar->drawTextMap(it->scout->getPosition(), "Scout of base");
      BWAPI::Broodwar->drawLineMap(it->scout->getPosition(), it->target->getCenter(), BWAPI::Colors::White);
      ++it;
    }
  }

  
  for (Base* base: bhaalBot->bases.bases)
    if ((base->status == Base::Status::Unknown ||
         base->status == Base::Status::Empty) &&
        base->accessibleOnGround)
    {
      if (!this->unassignedGroundScouters.empty() &&
          BWAPI::Broodwar->getFrameCount() - base->lastCheckedTick > scoutCooldown)
        {
          Unit* scout = this->unassignedGroundScouters.back();
          this->unassignedGroundScouters.pop_back();
          this->baseCheckTasks.push_back(CheckBaseTask(base, scout));
          base->lastCheckedTick = BWAPI::Broodwar->getFrameCount();
        }
      BWAPI::Position textPosition(base->getCenter());
      textPosition.y += 20;
      BWAPI::Broodwar->drawTextMap(textPosition,
                                   "Scout in %d seconds",
                                   (scoutCooldown - (BWAPI::Broodwar->getFrameCount() - base->lastCheckedTick)) / 24);
    }
}

Base* ScoutingManager::baseToScout()
{
  Base* bestCandidate = nullptr;
  for (Base* base: bhaalBot->bases.startingLocations)
  {
    if (base->startingBaseStatus == Base::StartingBaseStatus::Unknown &&
        !this->scoutAssigned(base) &&
        (bestCandidate == nullptr ||
         bestCandidate->getCenter().getDistance(bhaalBot->bases.startingBase->getCenter()) >
         base->getCenter().getDistance(bhaalBot->bases.startingBase->getCenter())))
        bestCandidate = base;
  }
  return bestCandidate;
}

bool ScoutingManager::scoutAssigned(Base* base)
{
  for (DiscoverScoutingLocationsScoutTask& task: this->scoutTasks)
    if (task.target == base)
      return true;
  return false;
}

void ScoutingManager::unassignScout(Unit* unit)
{
  for (uint32_t i = 0; i < this->unassignedGroundScouters.size(); ++i)
    if (this->unassignedGroundScouters[i] == unit)
    {
      this->unassignedGroundScouters.erase(this->unassignedGroundScouters.begin() + i);
      return;
    }

  for (uint32_t i = 0; i < this->scoutTasks.size(); ++i)
    if (this->scoutTasks[i].scout == unit)
    {
      this->scoutTasks.erase(this->scoutTasks.begin() + i);
      return;
    }

  for (uint32_t i = 0; i < this->baseCheckTasks.size();++i)
    if (this->baseCheckTasks[i].scout == unit)
    {
      this->baseCheckTasks.erase(this->baseCheckTasks.begin() + i);
      return;
    }
  LOG_AND_ABORT("Couldn't find the scout that was assigned.");
}

void ScoutingManager::orderToScout(BWAPI::UnitType unitType)
{
  this->ordersToScout[unitType]++;
}

void ScoutingManager::DiscoverScoutingLocationsScoutTask::onFrame()
{
  if (this->scout->getTargetPosition() != this->target->getCenter())
    this->scout->move(this->target->getCenter());
  if (this->scout->getPosition().getDistance(this->target->getCenter()) < 200)
  {
    BWAPI::Unitset nearbyEnemyBuildings = this->scout->getUnitsInRadius(200, BWAPI::Filter::IsEnemy && BWAPI::Filter::IsBuilding);
    if (!nearbyEnemyBuildings.empty())
    {
      this->target->startingBaseStatus = Base::StartingBaseStatus::Enemy;
      this->target->status = Base::Status::OwnedByEnemy;
    }
    else
    {
      this->target->startingBaseStatus = Base::StartingBaseStatus::Empty;
      this->target->status = Base::Status::Empty;
      bool enemyExists = false;
      uint32_t unknownCount = 0;
      for (Base* base: bhaalBot->bases.startingLocations)
        if (base->startingBaseStatus == Base::StartingBaseStatus::Enemy)
          enemyExists = true;
        else if (base->startingBaseStatus == Base::StartingBaseStatus::Unknown)
          ++unknownCount;
      if (!enemyExists && unknownCount == 1)
      {
        for (Base* base: bhaalBot->bases.bases)
          if (base->startingBaseStatus == Base::StartingBaseStatus::Unknown)
          {
            base->startingBaseStatus = Base::StartingBaseStatus::Enemy;
            base->status = Base::Status::OwnedByEnemy;
          }
      }
    }
    this->finished = true;
  }
}

void ScoutingManager::CheckBaseTask::onFrame()
{
  if (this->scout->getTargetPosition() != this->target->getCenter())
    this->scout->move(this->target->getCenter());
  if (this->scout == nullptr)
    this->finished = true;
  else
    if (this->scout->getDistance(this->target->getCenter()) < 100)
      this->finished = true;
}

ScoutTaskAssignment::~ScoutTaskAssignment()
{
  bhaalBot->scoutingManager.unassignScout(this->unit);
}
