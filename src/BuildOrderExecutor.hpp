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
  int getPlannedType(BWAPI::UnitType unitType);

  BuildOrder* currentBuildOrder = nullptr;
  uint32_t stepToDo = 0;
  bool automaticOverlordBuilding = false;
};
