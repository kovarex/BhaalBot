#pragma once
#include <BWAPI.h>
#include <set>
class Unit;

/** Wraps BWAPI::Player with our additional helper data. */
class Player
{
public:
  Player(BWAPI::Player player);
  void onUnitComplete(Unit* unit);
  void onUnitDestroy(Unit* unit);
  void onUnitMorph(Unit* unit, BWAPI::UnitType from);
  void drawDebug(BWAPI::Position position);
  int getUnitCount(BWAPI::UnitType unitType) const;
  int getUnitCountWithPlannedCombined(BWAPI::UnitType unitType) const;
  int getUnitCountWithPlannedAndBuildOrderCombined(BWAPI::UnitType unitType) const;
  
  std::map<BWAPI::UnitType, int> unitCounts;
  std::map<BWAPI::UnitType, int> overallLosses;
  std::set<Unit*> units;
  BWAPI::Player bwapiPlayer;

  bool isEnemy;
  bool myself;
};
