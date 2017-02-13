#include <Base.hpp>
#include <BuildTaskInProgress.hpp>
#include <BhaalBot.hpp>
#include <Unit.hpp>
#include <UnitSearch.hpp>
#include <Log.hpp>

BuildTaskInProgress::BuildTaskInProgress(BuildLocationType locationType,
                                         BWAPI::UnitType unitType)
 : CostReservationItem(Cost(unitType.mineralPrice(), unitType.gasPrice()))
 , locationType(locationType)
 , unitType(unitType)
 , frameOfLastOrder(BWAPI::Broodwar->getFrameCount())
{}

BuildTaskInProgress::~BuildTaskInProgress()
{
  if (this->worker)
    this->worker->assign(nullptr);
}

bool BuildTaskInProgress::onFrame()
{
  // This is special case of geysers, as they become the building instead of the drone in that case
  if (this->target && this->target->getType() == this->unitType && this->targetBuilding.isZero())
    this->targetBuilding = this->target;

  if (this->worker &&
      (this->worker->getType() == BWAPI::UnitTypes::Protoss_Probe ||
       this->worker->getType() == BWAPI::UnitTypes::Terran_SCV) &&
      this->targetBuilding.isZero())
  {
    for (Unit* unit: UnitSearch(this->position,
                                BWAPI::TilePosition(this->position.x + this->unitType.tileWidth(),
                                                    this->position.y + this->unitType.tileHeight())))
      if (unit->getType() == this->unitType)
      {
        this->targetBuilding = unit;
        if (this->worker->getType() == BWAPI::UnitTypes::Protoss_Probe)
        {
          Unit* worker = this->worker;
          worker->assign(nullptr);
          bhaalBot->moduleContainer.onUnitIdle(worker);
        }
        break;
      }
  }

  if (this->worker &&
      this->worker->getType() == BWAPI::UnitTypes::Zerg_Drone &&
      this->worker->getType() == this->unitType &&
      this->targetBuilding.isZero())
    this->targetBuilding = this->worker;

  if (!this->targetBuilding.isZero())
  {
    this->clear(); // Clears the cost reservation
    if (this->targetBuilding->getRemainingBuildTime() == 0)
    {
      bhaalBot->buildingPlaceabilityHelper.unRegisterBuild(unitType, this->position);
      if (this->worker)
      {
        Unit* worker = this->worker;
        worker->assign(nullptr);
        bhaalBot->moduleContainer.onUnitIdle(worker);
      }
      return true;
    }
    return false;
  }

  if (this->position == BWAPI::TilePositions::None)
  {
    this->position = this->getBuildPosition(&this->target);
    if (this->position == BWAPI::TilePositions::None)
      return false;
    bhaalBot->buildingPlaceabilityHelper.registerBuild(unitType, this->position);
  }

  if (this->worker == nullptr && this->targetBuilding.isZero())
  {
    this->worker = bhaalBot->harvestingManager.getClosestWorker(BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation()));
    this->worker->assign(new BuildTaskAssignment(this));
  }
  
  if (this->worker)
  {
    BWAPI::Order order = this->worker->getOrder();
    std::string name = order.toString();
    BWAPI::Position center = BWAPI::Position(this->position) + BWAPI::Position(this->unitType.tileSize()) / 2;
    if ((this->worker->isIdle() ||
         this->worker->isGatheringMinerals() ||
         BWAPI::Broodwar->getFrameCount() - this->frameOfLastOrder > 10) &&
        !worker->isConstructing())
    {
      uint32_t distance = 0;
      if (this->target)
        distance = this->worker->getDistance(this->target);
      else
        distance = this->worker->getDistance(center);
      if (distance > 30)
      {
        this->worker->move(center);
        this->frameOfLastOrder = BWAPI::Broodwar->getFrameCount();
      }
      else
      {
        if (BWAPI::Broodwar->self()->minerals() >= this->unitType.mineralPrice() &&
            BWAPI::Broodwar->self()->gas() >= this->unitType.gasPrice())
        {
          if (!this->worker->build(this->unitType, this->position))
          {
            bhaalBot->buildingPlaceabilityHelper.unRegisterBuild(unitType, this->position);
            this->position = this->getBuildPosition(&this->target);
            bhaalBot->buildingPlaceabilityHelper.registerBuild(unitType, this->position);
          }
          this->frameOfLastOrder = BWAPI::Broodwar->getFrameCount();
        }
      }
    }
    BWAPI::Broodwar->drawCircleMap(this->worker->getPosition(), 10, BWAPI::Colors::Red);
    BWAPI::Broodwar->drawLineMap(this->worker->getPosition(),
                                  (BWAPI::Position(this->position) +
                                  BWAPI::Position(this->position+ this->unitType.tileSize())) / 2,
                                  BWAPI::Colors::White);
  }
  BWAPI::Broodwar->drawBoxMap(BWAPI::Position(this->position),
                                              BWAPI::Position(this->position + this->unitType.tileSize()),
                                              BWAPI::Colors::Red);

  return false;
}

BWAPI::TilePosition BuildTaskInProgress::getBuildPosition(Unit** buildingTarget)
{
  switch (this->locationType)
  {
    case BuildLocationType::Auto:
      if (unitType == BWAPI::UnitTypes::Zerg_Extractor)
      {
        BaseHarvestingController::Geyser* freeGeyser = bhaalBot->harvestingManager.getFreeGeyser();
        if (!freeGeyser)
          return BWAPI::TilePositions::None;
        freeGeyser->state = BaseHarvestingController::Geyser::State::OrderToBuildOverGiven;
        *buildingTarget = freeGeyser->geyser;
        return freeGeyser->geyser->getBWAPIUnit()->getInitialTilePosition();
      }
      if (bhaalBot->harvestingManager.bases.empty())
        return BWAPI::TilePositions::None;
      return BWAPI::Broodwar->getBuildLocation(unitType, BWAPI::TilePosition(bhaalBot->harvestingManager.bases[0]->base->getCenter()));
    case BuildLocationType::NearFirstExpansion:
    {
      if (bhaalBot->harvestingManager.bases.size() < 2)
        return BWAPI::TilePositions::None;
        BWAPI::TilePosition startLocation = BWAPI::TilePosition(bhaalBot->harvestingManager.bases[1]->againstMinerals());
        if (startLocation == BWAPI::TilePositions::None)
          return startLocation;
        if (bhaalBot->buildingPlaceabilityHelper.canBuild(unitType, startLocation, this->worker))
          return startLocation;
        for (int32_t radius = 1; radius < 5; ++radius)
        {
          for (int32_t i = 0; i < radius + 1; ++i)
          {
            for (BWAPI::TilePosition position: {BWAPI::TilePosition(startLocation.x - radius + i, startLocation.y - radius),
                                                BWAPI::TilePosition(startLocation.x + radius, startLocation.y - radius + i),
                                                BWAPI::TilePosition(startLocation.x + radius - i, startLocation.y + radius),
                                                BWAPI::TilePosition(startLocation.x - radius, startLocation.y + radius - i)})
              if (bhaalBot->buildingPlaceabilityHelper.canBuild(unitType, position, this->worker))
                return position;
          }
        }
        return BWAPI::TilePositions::None;
    }
    case BuildLocationType::ClosestExpansion:
    {
      auto& theMap = BWEM::Map::Instance();
      const BWEM::Base* bestBase = nullptr;
      float bestDistance = 0;
      for (const BWEM::Area& area: theMap.Areas())
        for (const BWEM::Base& base: area.Bases())
        {
          if (bhaalBot->harvestingManager.hasBaseNearby(base.Center()))
            continue;
          float distance = bhaalBot->harvestingManager.averageDistanceToBases(base.Center());
          if (distance < 0)
            continue;
          if (bestBase == nullptr || distance < bestDistance)
          {
            bestBase = &base;
            bestDistance = distance;
          }
        }
      if (!bestBase)
      {
        BWAPI::Broodwar->sendText("Didn't find expansion candidate, this is a problem ....");
        return BWAPI::TilePositions::None;
      }
      return bestBase->Location();
    }
  }
  return BWAPI::TilePositions::None;
}

void BuildTaskInProgress::unassign(Unit* unit)
{
  if (this->worker != unit)
    LOG_AND_ABORT("Trying to unassign wrong unit.");
  this->worker = nullptr;
}

BuildTaskAssignment::~BuildTaskAssignment()
{
  this->buildTask->unassign(this->unit);
}

std::string BuildTaskAssignment::str() const
{
  return "Build " + Unit::shortenUnitName(this->buildTask->unitType.getName());
}
