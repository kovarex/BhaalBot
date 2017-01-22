#pragma once
#include <BWAPI.h>
#include <set>
class Base;

class Bases
{
public:
  Bases();
  void init(); /**< it has to be done later than in the constructor so it can use the initialised BWEM */
  Base* enemyMainBase();
  Base* getClosestBase(BWAPI::Position position);

private:
  void initAccessibilityFromStartingBase();
  void initStartingLocations();
public:
  std::set<Base*> bases;
  Base* startingBase = nullptr;
  std::set<Base*> startingLocations;
};
