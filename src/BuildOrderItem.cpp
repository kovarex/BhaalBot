#include <BWEM/bwem.h>
#include <BuildOrderItem.hpp>
#include <BaseHarvestingController.hpp>
#include <BhaalBot.hpp>
#include <StringUtil.hpp>
#include <Player.hpp>
#include <BuildTaskInProgress.hpp>
#include <Unit.hpp>
#include <Log.hpp>

bool BuildBuildOrderItem::execute()
{
  BWAPI::UnitType unitType = this->unit;
  if (!bhaalBot->costReservation.canAfford(Cost(unitType.mineralPrice(), unitType.gasPrice())))
    return false;

  if (unitType == BWAPI::UnitTypes::Zerg_Lair)
  {
    if ( bhaalBot->harvestingManager.bases.empty())
      return false;
    Unit* target =  bhaalBot->harvestingManager.bases[0]->baseUnit;
    return target->morph(unitType);
  }
  if (unitType == BWAPI::UnitTypes::Zerg_Sunken_Colony ||
      unitType == BWAPI::UnitTypes::Zerg_Spore_Colony)
  {
    for (Unit* unit: bhaalBot->players.self->units)
      if (unit->getType() == BWAPI::UnitTypes::Zerg_Creep_Colony &&
          unit->getRemainingBuildTime() == 0 &&
          !bhaalBot->morphingUnits.isScheduledToBeMorphed(unit))
      {
        if (unit->morph(unitType))
        {
          LOG_INFO("Morphing %s %d to %s",
                   unit->getName().c_str(),
                   unit->getID(),
                   Unit::shortenUnitName(unitType.getName()).c_str());
          bhaalBot->morphingUnits.planMorphOf(unit, unitType);
          return true;
        }
        return false;
      }
    return false;
  }

  if (unitType.isBuilding())
  {
     bhaalBot->buildTasks.buildTasksInProgress.push_back(new BuildTaskInProgress(this->location, unitType));
    return true;
  }
  else
  {
    if (unitType.supplyRequired() + BWAPI::Broodwar->self()->supplyUsed() > BWAPI::Broodwar->self()->supplyTotal())
      return false;
    return bhaalBot->buildOrderManager.executor.train(unitType);
  }
}

std::string BuildBuildOrderItem::str() const
{
  return ssprintf("%s", Unit::shortenUnitName(this->unit.getName()).c_str());
}

bool SendScoutBuildOrderItem::execute()
{
  bhaalBot->scoutingManager.orderToScout(this->unitType);
  return true;
}


bool SwitchToAutomaticOverlordBuilding::execute()
{
  bhaalBot->buildOrderManager.executor.automaticOverlordBuilding = true;
  return true;
}
