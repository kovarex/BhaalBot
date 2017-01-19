#include <MorphingUnits.hpp>
#include <Unit.hpp>

MorphingUnits::MorphingUnits(ModuleContainer& moduleContainer)
 : Module(moduleContainer)
{}

void MorphingUnits::onUnitMorph(Unit* unit)
{
  if (unit->getPlayer() != BWAPI::Broodwar->self())
    return;
  if (unit->getType() == BWAPI::UnitTypes::Zerg_Egg)
    this->counts[unit->getBuildType()]++;
  else if (!unit->getType().isBuilding())
  {
    std::string message = unit->getType().getName();
    if (this->counts[unit->getType()] == 0)
      throw std::runtime_error("Morph count of something would go under 0");
    this->counts[unit->getType()]--;
  }
}

uint32_t MorphingUnits::getMorphingCount(BWAPI::UnitType unitType)
{
  auto position = this->counts.find(unitType);
  if (position == this->counts.end())
    return 0;
  return position->second;
}
