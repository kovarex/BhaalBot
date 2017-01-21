#include <Units.hpp>
#include <Unit.hpp>

Unit* Units::onUnitComplete(BWAPI::Unit unit)
{
  Unit* ourUnit = new Unit(unit);
  this->units[unit] = ourUnit;
  return ourUnit;
}

void Units::onUnitDestroy(BWAPI::Unit unit)
{
  auto position = this->units.find(unit);
  if (position == this->units.end())
    return;
  delete position->second;
  this->units.erase(position);
}

Unit* Units::find(BWAPI::Unit unit)
{
  auto position = this->units.find(unit);
  if (position == this->units.end())
    return nullptr;
  return position->second;
}

void Units::printAssignments()
{
  for (auto& item: this->units)
    item.second->printAssignment();
}

Unit* Units::findOrThrow(BWAPI::Unit unit)
{
  auto position = this->units.find(unit);
  if (position == this->units.end())
    throw std::runtime_error("Trying to find unknown " + unit->getType().getName());
  return position->second;
}
