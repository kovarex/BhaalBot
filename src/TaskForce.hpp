#pragma once
#include <BWAPI.h>
#include <set>
class Unit;

class TaskForce
{
public:
  void addUnit(Unit* unit);
protected:
  virtual void onAdded(Unit*) {}
private:
  std::set<Unit*> units;
};
