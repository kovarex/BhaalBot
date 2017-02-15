#pragma once
#include <UnitCompositionSelector/UnitCompositionSelector.hpp>

class ZvPUnitCompositionSelector : public UnitCompositionSelector
{
public:
  virtual BWAPI::UnitType whatToBuild(const GameStateRelatedToUnitComposition& gameState);
  virtual UnitCompositionSelector* clone() const { return new ZvPUnitCompositionSelector(); }
};
