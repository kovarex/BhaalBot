#include <TaskForce.hpp>

void TaskForce::addUnit(Unit* unit)
{
  units.insert(unit);
  this->onAdded(unit);
}
