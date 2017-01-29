#pragma once
#include <GroupController.hpp>
#include <set>
class Base;

class DroneDefenseGroupController : public GroupController
{
public:
  DroneDefenseGroupController(Group& owner, Base* base);
  void onAdded(Unit* unit);
  virtual void onRemoved(Unit* unit);
  virtual void onFrame();
private:
  bool isOccupiedByMineral(BWAPI::Position position);
  BWAPI::Unit getBestMineralForDefense(bool drawDebugInfo = false);
public:
  Base* base;
  std::set<Unit*> stacked;
  std::set<Unit*> goingToMineral;
  BWAPI::Unit defenseMineral = nullptr;
};
