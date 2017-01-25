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
  if (unit->canGather())
  {
    LOG_INFO("New drone assigned to gather");
    bhaalBot->harvestingManager.add(unit);
  }
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
          base->defenseState = Base::DefenseState::Defending;
          LOG_NOTICE("Switching base to defense mode.");
        }
      }
      else if (base->baseInDangerDetector->dangerLevel < 2)
      {
        if (base->defenseState == Base::DefenseState::Defending)
        {
          base->defenseState = Base::DefenseState::None;
          LOG_NOTICE("Switching base to normal mode.");
        }
      }
}
