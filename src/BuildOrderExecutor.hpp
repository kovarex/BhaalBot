#pragma once
#include <BWAPI.h>
#include <Cost.hpp>
#include <BuildLocationType.hpp>
#include <CostReservation.hpp>
class BuildTaskInProgress;
class BhaalBot;
class BuildOrder;

class BuildOrderExecutor
{
public:
  BuildOrderExecutor() {}
  ~BuildOrderExecutor();
  void startBuildOrder(BuildOrder* buildOrder);
  void update();
  bool train(BWAPI::UnitType unitType);
  bool isFinished();
  /**< How many units of this type is planned in the build order (counting only items that were not executed yet). */
  int getPlannedType(BWAPI::UnitType unitType);
  /**< How much money is to be spent in the build order (counting only items that were not executed yet). */
  Cost getPlannedCost();

  BuildOrder* currentBuildOrder = nullptr;
  uint32_t stepToDo = 0;
  bool automaticSupplyBuilding = false;
};
