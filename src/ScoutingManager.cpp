#include <ScoutingManager.hpp>
#include <BhaalBot.hpp>
#include <Unit.hpp>
#include <Base.hpp>

ScoutingManager::ScoutingManager(ModuleContainer& moduleContainer)\
  : Module(moduleContainer)
{}

void ScoutingManager::assignGroundScout(Unit* unit)
{
  this->unassignedGroundScouters.push_back(unit);
  unit->assign(new ScoutTaskAssignment());
}

void ScoutingManager::onFrame()
{
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
      this->unassignedGroundScouters.push_back(task.scout);
      this->scoutTasks.erase(this->scoutTasks.begin() + i);
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
      if (it->scout != nullptr)
        this->unassignedGroundScouters.push_back(it->scout);
      it = this->baseCheckTasks.erase(it);
    }
    else
    {
      BWAPI::Broodwar->drawTextMap(it->scout->getPosition(), "Scout of base");
      BWAPI::Broodwar->drawLineMap(it->scout->getPosition(), it->target->getCenter(), BWAPI::Colors::White);
      ++it;
    }
  }

  
  for (Base* base: bhaalBot->bases.bases)
    if (base->status == Base::Status::Unknown ||
        base->status == Base::Status::Empty)
    {
      if (!this->unassignedGroundScouters.empty() &&
          BWAPI::Broodwar->getFrameCount() - base->lastCheckedTick > scoutCooldown)
        {
          Unit* scout = this->unassignedGroundScouters.back();
          this->unassignedGroundScouters.pop_back();
          this->baseCheckTasks.push_back(CheckBaseTask(base, scout));
          base->lastCheckedTick = BWAPI::Broodwar->getFrameCount();
        }
      BWAPI::Broodwar->drawTextMap(base->getCenter(),
                                   "Scout in %d seconds",
                                   (scoutCooldown - (BWAPI::Broodwar->getFrameCount() - base->lastCheckedTick)) / 24);
    }
}

Base* ScoutingManager::baseToScout()
{
  for (Base* base: bhaalBot->bases.startingLocations)
  {
    if (base->startingBaseStatus == Base::StartingBaseStatus::Unknown &&
        !this->scoutAssigned(base))
      return base;
  }
  return nullptr;
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
  throw std::runtime_error("Couldn't find the scout that was assigned.");
}

void ScoutingManager::DiscoverScoutingLocationsScoutTask::onFrame()
{
  if (this->scout->getTargetPosition() != this->target->getCenter())
    this->scout->move(this->target->getCenter());
  if (this->scout->getPosition().getDistance(this->target->getCenter()) < 50)
  {
    BWAPI::Unitset nearbyUnits = this->scout->getUnitsInRadius(100);
    bool containsBuilding = false;
    for (BWAPI::Unit nearbyUnit: nearbyUnits)
      if (nearbyUnit->getType().isBuilding())
        containsBuilding = true;
    if (containsBuilding)
      this->target->startingBaseStatus = Base::StartingBaseStatus::Enemy;
    else
    {
      this->target->startingBaseStatus = Base::StartingBaseStatus::Empty;
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
            base->startingBaseStatus = Base::StartingBaseStatus::Enemy;
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
