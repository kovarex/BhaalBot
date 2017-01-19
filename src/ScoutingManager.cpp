#include <ScoutingManager.hpp>
#include <BhaalBot.hpp>
#include <Unit.hpp>

ScoutingManager::ScoutingManager(ModuleContainer& moduleContainer)\
  : Module(moduleContainer)
{}

void ScoutingManager::assignGroundScout(Unit* unit)
{
  this->unassignedGroundScouters.push_back(unit);
}

void ScoutingManager::onFrame()
{
  if (!this->initialised && !bhaalBot->harvestingManager.bases.empty())
  {
    for (BWAPI::TilePosition startingLocation: BWEM::Map::Instance().StartingLocations())
    {
      const BWEM::Base* base = this->getClosestBase(BWAPI::Position(startingLocation));

      if (bhaalBot->harvestingManager.bases[0]->bwemBase == base)
      {
        this->startingLocations[base] = StartingLocationState::Self;
        this->startingBase = base;
      }
      else
        this->startingLocations[base] = StartingLocationState::Unknown;
    }
    for (const BWEM::Area& area: BWEM::Map::Instance().Areas())
      for (const BWEM::Base& base: area.Bases())
      {
        if (&base == this->startingBase)
          continue;
        int length = 0;
        auto& path = BWEM::Map::Instance().GetPath(this->startingBase->Center(), base.Center(), &length);
        if (length == -1)
          continue;
        this->bases[&base] = BaseScoutingData();
      }
    this->initialised = true;
  }

  while (!this->unassignedGroundScouters.empty())
  {
    const BWEM::Base* base = this->baseToScout();
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
      BWAPI::Broodwar->drawLineMap(task.scout->getPosition(), task.target->Center(), BWAPI::Colors::White);
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
      BWAPI::Broodwar->drawLineMap(it->scout->getPosition(), it->target->Center(), BWAPI::Colors::White);
      ++it;
    }
  }

  
  for (auto& item: bases)
  {
    if (!this->unassignedGroundScouters.empty())
    {
      if (BWAPI::Broodwar->getFrameCount() - item.second.lastCheckedTick > scoutCooldown)
      {
        Unit* scout = this->unassignedGroundScouters.back();
        this->unassignedGroundScouters.pop_back();
        this->baseCheckTasks.push_back(CheckBaseTask(item.first, scout));
        item.second.lastCheckedTick = BWAPI::Broodwar->getFrameCount();
      }
    }
    BWAPI::Broodwar->drawTextMap(item.first->Center(),
                                 "Scout in %d seconds",
                                 (scoutCooldown - (BWAPI::Broodwar->getFrameCount() - item.second.lastCheckedTick)) / 24);
  }
}

const BWEM::Base* ScoutingManager::getClosestBase(BWAPI::Position position)
{
  for (const BWEM::Area& area: BWEM::Map::Instance().Areas())
    for (const BWEM::Base& base: area.Bases())
      if (base.Center().getDistance(position) < 100)
        return &base;
  return nullptr;
}

const BWEM::Base* ScoutingManager::baseToScout()
{
  for (auto item: this->startingLocations)
  {
    if (item.second == StartingLocationState::Unknown && !this->scoutAssigned(item.first))
      return item.first;
  }
  return nullptr;
}

bool ScoutingManager::scoutAssigned(const BWEM::Base* base)
{
  for (DiscoverScoutingLocationsScoutTask& task: this->scoutTasks)
    if (task.target == base)
      return true;
  return false;
}

void ScoutingManager::onUnitDestroy(Unit* unit)
{
  for (uint32_t i = 0; i < this->unassignedGroundScouters.size(); ++i)
    if (this->unassignedGroundScouters[i] == unit)
      this->unassignedGroundScouters.erase(this->unassignedGroundScouters.begin() + i);
    else
      ++i;

  for (uint32_t i = 0; i < this->scoutTasks.size(); ++i)
    if (this->scoutTasks[i].scout == unit)
      this->scoutTasks.erase(this->scoutTasks.begin() + i);
    else
      ++i;

  for (uint32_t i = 0; i < this->baseCheckTasks.size(); ++i)
    if (this->baseCheckTasks[i].scout == unit)
      this->baseCheckTasks.erase(this->baseCheckTasks.begin() + i);
    else
      ++i;
}

BWAPI::Position ScoutingManager::enemyMainBase()
{
  for (auto& item: this->startingLocations)
    if (item.second == StartingLocationState::Enemy)
      return item.first->Center();
  return BWAPI::Positions::Unknown;
}

void ScoutingManager::DiscoverScoutingLocationsScoutTask::onFrame()
{
  if (this->scout->getTargetPosition() != this->target->Center())
    this->scout->move(this->target->Center());
  if (this->scout->getPosition().getDistance(this->target->Center()) < 50)
  {
    BWAPI::Unitset nearbyUnits = this->scout->getUnitsInRadius(100);
    bool containsBuilding = false;
    for (BWAPI::Unit nearbyUnit: nearbyUnits)
      if (nearbyUnit->getType().isBuilding())
        containsBuilding = true;
    if (containsBuilding)
      bhaalBot->scoutingManager.startingLocations[this->target] = StartingLocationState::Enemy;
    else
    {
      bhaalBot->scoutingManager.startingLocations[this->target] = StartingLocationState::Empty;
      bool enemyExists = false;
      uint32_t unknownCount = 0;
      for (auto& item: bhaalBot->scoutingManager.startingLocations)
        if (item.second == StartingLocationState::Enemy)
          enemyExists = true;
        else if (item.second == StartingLocationState::Unknown)
          ++unknownCount;
      if (!enemyExists && unknownCount == 1)
      {
        for (auto& item: bhaalBot->scoutingManager.startingLocations)
          if (item.second == StartingLocationState::Unknown)
            item.second = StartingLocationState::Enemy;
      }
    }
    this->finished = true;
  }
}

void ScoutingManager::CheckBaseTask::onFrame()
{
  if (this->scout->getTargetPosition() != this->target->Center())
    this->scout->move(this->target->Center());
  if (this->scout == nullptr)
    this->finished = true;
  else
    if (this->scout->getDistance(this->target->Center()) < 100)
      this->finished = true;
}
