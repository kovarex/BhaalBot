#pragma once
#include <TaskForceController.hpp>

class DefendTaskForceController : public TaskForceController
{
public:
  DefendTaskForceController(TaskForce& owner, Base* base);
  void assignUnit(Unit* unit) override;

  Base* base;
  Group* theGroup;
  Group* droneDefenseGroup;
};
