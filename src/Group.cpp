#include <Group.hpp>
#include <GroupController.hpp>

void Group::add(Unit* unit)
{
  this->units.insert(unit);
  this->controller->onAdded(unit);
}

void Group::remove(Unit* unit)
{
  this->units.erase(unit);
  this->controller->onRemoved(unit);
}

void Group::onFrame()
{
  if (this->controller)
    this->controller->onFrame();
}
