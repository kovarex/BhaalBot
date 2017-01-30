#pragma once
#include <BWAPI.h>
#include <set>
class Unit;

class Player
{
public:
  Player(BWAPI::Player player);
  void onUnitComplete(Unit* unit);
  void onUnitDestroy(Unit* unit);
  void onUnitMorph(Unit* unit, BWAPI::UnitType from);
  void drawDebug(BWAPI::Position position);
  
  std::map<BWAPI::UnitType, int> unitCounts;
  std::map<BWAPI::UnitType, int> overallLosses;
  std::set<Unit*> units;
  BWAPI::Player bwapiPlayer;

  bool isEnemy;
  bool myself;
};
