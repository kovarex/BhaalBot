#pragma once
#include <BaseHarvestingController.hpp>
#include <Module.hpp>
#include <TaskForce.hpp>
class ModuleContainer;

class HarvestingManager : public Module
{
public:
  HarvestingManager(ModuleContainer& moduleContainer);
  ~HarvestingManager();

  void add(Unit* unit);
public:
  void addBase(Unit* baseUnit, Base* base);
  Unit* getClosestWorker(BWAPI::Position position);
  bool hasBaseNearby(BWAPI::Position position) const;
  float averageDistanceToBases(BWAPI::Position position) const;
  BaseHarvestingController::Geyser* getFreeGeyser();
  void balanceGasMining();
  void balanceWorkersAssignmentBetweenBases();

  virtual void onUnitComplete(Unit* unit) override;
  virtual void onFrame() override;

  std::vector<Unit*> baseLessMiners;
  std::vector<BaseHarvestingController*> bases;
  uint32_t geyserMinersWanted = uint32_t(-1); // uint32_t(-1) is infinity, 0 means don't mine gas, 2 means 2 miners etc...
};
