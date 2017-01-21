#include <Overmind.hpp>
#include <BhaalBot.hpp>
#include <Unit.hpp>

Overmind::Overmind(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

void Overmind::onUnitComplete(Unit* unit)
{
  if (unit->canGather())
    bhaalBot->harvestingManager.add(unit);
}
