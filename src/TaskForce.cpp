#include <TaskForce.hpp>
#include <Group.hpp>
#include <TaskForceController.hpp>

TaskForce::~TaskForce()
{
  for (Group* group: this->groups)
    delete group;
  delete this->taskForceController;
}

TaskForce::TaskForce()
{}

void TaskForce::addUnit(Unit* unit)
{
  if (this->groups.empty())
    throw std::runtime_error("No groups to add.");
  (*this->groups.begin())->add(unit);
  this->onAdded(unit);
}

void TaskForce::assignTaskController(TaskForceController* taskForceController)
{
  delete this->taskForceController;
  this->taskForceController = taskForceController;
}

std::vector<Unit*> TaskForce::removeGroup(Group* group)
{
  std::vector<Unit*> result;
  for (Unit* unit: group->getUnits())
    result.push_back(unit);
  delete group;
  this->groups.erase(group);
  return result;
}

std::vector<Unit*> TaskForce::removeAllGroups()
{
  std::vector<Unit*> units;
  while (!this->groups.empty())
  {
    std::vector<Unit*> groupUnits = this->removeGroup(*this->groups.begin());
    for (Unit* unit: groupUnits)
      units.push_back(unit);
  }
  return units;
}

Group* TaskForce::createGroup()
{
  Group* group = new Group();
  this->groups.insert(group);
  return group;
}

void TaskForce::onFrame()
{
  if (this->taskForceController != nullptr)
    this->taskForceController->onFrame();
  for (Group* group: this->groups)
    group->onFrame();
}

Unit* TaskForce::getUnit(BWAPI::UnitType unitType)
{
  for (Group* group: this->groups)
    if (Unit* result = group->getUnit(unitType))
      return result;
}
