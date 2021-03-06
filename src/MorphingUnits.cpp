#include <BhaalBot.hpp>
#include <MorphingUnits.hpp>
#include <Unit.hpp>
#include <Log.hpp>

MorphingUnits::MorphingUnits(ModuleContainer& moduleContainer)
 : Module(moduleContainer)
{}

void MorphingUnits::onUnitMorph(Unit* unit, BWAPI::UnitType from)
{
  if (unit->getPlayer() != bhaalBot->players.self)
    return;
  if (unit->getType() == BWAPI::UnitTypes::Zerg_Egg)
    this->counts[unit->getBuildType()]++;
  else if (!unit->getType().isBuilding())
  {
    std::string message = unit->getType().getName();
    if (this->counts[unit->getType()] == 0)
      LOG_AND_ABORT("Morph count of something would go under 0");
    this->counts[unit->getType()]--;
  }
  LOG_INFO("%s %d morphed from %s", unit->getType().getName().c_str(), unit->getID(), from.getName().c_str());
  this->thingsToBeMorphed.erase(unit);
}

uint32_t MorphingUnits::getPlannedCount(BWAPI::UnitType unitType)
{
  return this->getMorphingCount(unitType) + this->getPlannedMorphsCount(unitType);
}

void MorphingUnits::onFrame()
{
  for (auto& item: this->thingsToBeMorphed)
    if (BWAPI::Broodwar->getFrameCount() - item.second.frameOfLastOrder > BWAPI::Broodwar->getLatencyFrames())
      item.first->train(item.second.targetType);
}

void MorphingUnits::planMorphOf(Unit* unit, BWAPI::UnitType targetType)
{
  this->thingsToBeMorphed[unit] = UnitToBeMorphedData(targetType);
}

uint32_t MorphingUnits::getMorphingCount(BWAPI::UnitType unitType)
{
  auto position = this->counts.find(unitType);
  if (position == this->counts.end())
    return 0;
  return position->second;
}

uint32_t MorphingUnits::getPlannedMorphsCount(BWAPI::UnitType unitType)
{
    auto position = this->plannedMorphs.find(unitType);
  if (position == this->plannedMorphs.end())
    return 0;
  return position->second;
}

MorphingUnits::UnitToBeMorphedData::UnitToBeMorphedData(BWAPI::UnitType targeType)
  : targetType(targeType)
  , frameOfLastOrder(BWAPI::Broodwar->getFrameCount())
{}
