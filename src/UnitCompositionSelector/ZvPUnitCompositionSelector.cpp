#include <UnitCompositionSelector/ZvPUnitCompositionSelector.hpp>
#include <Util.hpp>

BWAPI::UnitType ZvPUnitCompositionSelector::whatToBuild(const GameStateRelatedToUnitComposition& gameState)
{
  // more of an example than any meaningful logic.
  if (gameState.availableUnits.count(BWAPI::UnitTypes::Zerg_Zergling) &&
      getMapCount(gameState.myUnits, BWAPI::UnitTypes::Zerg_Zergling) < 12)
    return BWAPI::UnitTypes::Zerg_Zergling;
  if (gameState.availableUnits.count(BWAPI::UnitTypes::Zerg_Hydralisk))
    return BWAPI::UnitTypes::Zerg_Hydralisk;
  if (gameState.availableUnits.count(BWAPI::UnitTypes::Zerg_Zergling))
    return BWAPI::UnitTypes::Zerg_Zergling;
  return BWAPI::UnitTypes::None;
}
