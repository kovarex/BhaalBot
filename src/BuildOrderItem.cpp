#include <BWEM/bwem.h>
#include <BuildOrderItem.hpp>
#include <BaseHarvestingManager.hpp>
#include <BhaalBot.hpp>
#include <StringUtil.hpp>
#include <BuildTaskInProgress.hpp>
#include <Unit.hpp>

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
    for (BWAPI::Unit unit: BWAPI::Broodwar->self()->getUnits())
      if (unit->getType() == BWAPI::UnitTypes::Zerg_Creep_Colony &&
          unit->getRemainingBuildTime() == 0)
        return unit->morph(unitType);
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
    return  bhaalBot->buildOrderManager.executor.train(unitType);
  }
}

std::string BuildBuildOrderItem::str() const
{
  return ssprintf("%s for %u/%u", this->unit.getName().c_str(), this->unit.mineralPrice(), this->unit.gasPrice());
}

bool SendScoutBuildOrderItem::execute()
{
  Unit* unit = bhaalBot->fightManager.getUnit(this->unitType);
  if (unit)
  {
    unit->assign(nullptr);
    bhaalBot->scoutingManager.assignGroundScout(unit);
  }
  return true;
}


bool SwitchToAutomaticOverlordBuilding::execute()
{
  bhaalBot->buildOrderManager.executor.automaticOverlordBuilding = true;
  return true;
}
