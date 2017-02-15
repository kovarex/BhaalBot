#pragma once
#include <BWAPI.h>
#include <Cost.hpp>
#include <map>
#include <set>

class GameStateRelatedToUnitComposition
{
public:
  GameStateRelatedToUnitComposition(const std::map<BWAPI::UnitType, int>& myUnits,
                                    const std::map<BWAPI::UnitType, int>& enemyUnits,
                                    const std::set<BWAPI::UnitType>& availableUnits,
                                    Cost availableResources)
    : myUnits(myUnits)
    , enemyUnits(enemyUnits)
    , availableUnits(availableUnits)
    , availableResources(availableResources) {}

  const std::map<BWAPI::UnitType, int>& myUnits;
  const std::map<BWAPI::UnitType, int>& enemyUnits;
  const std::set<BWAPI::UnitType> availableUnits;
  Cost availableResources;
};

class UnitCompositionSelector
{
public:
  virtual BWAPI::UnitType whatToBuild(const GameStateRelatedToUnitComposition& gameState) = 0;
  virtual UnitCompositionSelector* clone() const = 0;
};
