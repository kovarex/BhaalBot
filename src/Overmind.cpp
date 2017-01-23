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
