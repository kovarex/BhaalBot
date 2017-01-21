#include <TaskForce.hpp>
#include <Group.hpp>

TaskForce::~TaskForce()
{
  for (Group* group: this->groups)
    delete group;
  delete this->taskForceController;
}

TaskForce::TaskForce(TaskForceController* taskForceController)
  : taskForceController(taskForceController)
{
  this->groups.insert(new Group());
}

void TaskForce::addUnit(Unit* unit)
{
  if (this->groups.empty())
    throw std::runtime_error("No groups to add.");
  (*this->groups.begin())->add(unit);
  this->onAdded(unit);
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
