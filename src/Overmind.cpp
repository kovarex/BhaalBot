#include <Base.hpp>
#include <BaseInDangerDetector.hpp>
#include <Overmind.hpp>
#include <BhaalBot.hpp>
#include <Unit.hpp>
#include <Log.hpp>

Overmind::Overmind(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

void Overmind::onUnitComplete(Unit* unit)
{
  if (unit->getPlayer() == bhaalBot->players.self)
    this->assignUnit(unit);
}

void Overmind::onUnitIdle(Unit* unit)
{
  this->assignUnit(unit);
}

void Overmind::onUnitDestroy(Unit* unit)
{
  LOG_INFO("My %s destroyed", unit->getType().getName().c_str());
}

void Overmind::onFrame()
{
  for (Base* base: bhaalBot->bases.bases)
    if (base->baseInDangerDetector)
      if (base->baseInDangerDetector->dangerLevel > 1)
      {
        if (base->defenseState == Base::DefenseState::None)
        {
          //BWAPI::Broodwar->setLocalSpeed(200);
          base->defenseState = Base::DefenseState::Defending;
          LOG_NOTICE("Switching base to defense mode.");
          while (Unit* worker = base->harvestingController->getLeastNeededWorker())
            base->defendForce->addUnit(worker);
        }
      }
      else if (base->baseInDangerDetector->dangerLevel < 2)
      {
        if (base->defenseState == Base::DefenseState::Defending)
        {
          base->defenseState = Base::DefenseState::None;
          for (BWAPI::UnitType unitType: {BWAPI::UnitTypes::Zerg_Drone,
                                          BWAPI::UnitTypes::Protoss_Probe,
                                          BWAPI::UnitTypes::Terran_SCV})
            while (Unit* drone = base->defendForce->getUnit(unitType))
              if (base->harvestingController)
                base->harvestingController->assignMiner(drone);
              else
                bhaalBot->harvestingManager.add(drone);
          LOG_NOTICE("Switching base to normal mode.");
        }
      }
}

void Overmind::assignUnit(Unit* unit)
{
  if (unit->canGather())
  {
    Base* base = bhaalBot->bases.getClosestBase(unit->getPosition());
    if (!base)
      return; // no base this is probably bad :)
    if (base->defenseState == Base::DefenseState::None)
    {
      LOG_INFO("Drone assigned to gather");
      bhaalBot->harvestingManager.add(unit);
    }
    else
    {
      LOG_INFO("Drone assigned to defend");
      base->defendForce->addUnit(unit);
    }
  }
}
