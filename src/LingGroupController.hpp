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

  void onAdded(Unit* ling);
  //void onFrame();

  //void attackTarget(BWAPI::Unit unit);
  void chooseTarget(); // TODO maybe give Unit* parameter
};
