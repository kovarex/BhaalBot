#pragma once
#include <EnemyUnitTarget.hpp>
#include <GroupController.hpp>
#include <BWAPI.h>
#include <vector>
class Unit;
class UnitMemoryInfo;

class LingGroupController : public GroupController
{
public:
  LingGroupController(Group& owner);
  virtual void setAttackTarget(BWAPI::Unit target) override;
  UnitMemoryInfo* getAttackTarget() override { return nullptr; }

  void onAdded(Unit* ling);
  void onFrame();
  void setTargetUnit(BWAPI::Unit targetUnit) { this->targetUnit = targetUnit;  }
  void setTargetPosition(BWAPI::Position targetPosition) { this->targetPosition = targetPosition; }

  BWAPI::Position getCenter();
  //void attackTarget(BWAPI::Unit unit);
  //void move(); // moves to target position or unit
  //void attack();  // attacks target position or unit
  void chooseTarget(); // TODO maybe give Unit* parameter

  BWAPI::Position targetPosition;
  BWAPI::Unit targetUnit;
};
