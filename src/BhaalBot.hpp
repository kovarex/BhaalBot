#pragma once
#include <BWAPI.h>
#include <HarvestingManager.hpp>
#include <BuildOrderManager.hpp>
#include <Module.hpp>
#include <ModuleContainer.hpp>
#include <DeleyedExitChecker.hpp>
#include <MorphingUnits.hpp>
#include <ProducingUnits.hpp>
#include <BuildingPlaceabilityHelper.hpp>
#include <FightManager.hpp>
#include <CostReservation.hpp>
#include <ProducerManager.hpp>
#include <LarvaReservations.hpp>
#include <ScoutingManager.hpp>
#include <DiscoveredMemory.hpp>
#include <BuildTasks.hpp>
#include <DangerZones.hpp>
#include <Overmind.hpp>
#include <Bases.hpp>
#include <Units.hpp>
#include <Logger.hpp>
#include <PropertyTree.hpp>
#include <Players.hpp>
#include <Strategy/StrategyExecutor.hpp>
#include <Strategy/StrategySelector.hpp>
#include <ProductionQueueReservations.hpp>
#include <random>

class BhaalBot : public BWAPI::AIModule
{
public:
  BhaalBot();
  // Virtual functions for callbacks, leave these as they are.
  virtual void onStart();
  virtual void onEnd(bool isWinner);
  virtual void onFrame();
  virtual void onSendText(std::string text);
  virtual void onReceiveText(BWAPI::Player player, std::string text);
  virtual void onPlayerLeft(BWAPI::Player player);
  virtual void onNukeDetect(BWAPI::Position target);
  virtual void onUnitDiscover(BWAPI::Unit unit);
  virtual void onUnitEvade(BWAPI::Unit unit);
  virtual void onUnitShow(BWAPI::Unit unit);
  virtual void onUnitHide(BWAPI::Unit unit);
  virtual void onUnitCreate(BWAPI::Unit unit);
  virtual void onUnitDestroy(BWAPI::Unit unit);
  virtual void onUnitMorph(BWAPI::Unit unit);
  virtual void onUnitRenegade(BWAPI::Unit unit);
  virtual void onUnitComplete(BWAPI::Unit unit);
  bool BhaalBot::isUMSMap();
  // Everything below this line is safe to modify.

  std::random_device randomDevice;
  Logger logger;
  PropertyTree config;
  ModuleContainer moduleContainer;
  Overmind overmind;
  Bases bases;
  DangerZones dangerZones;
  Units units;
  Players players;
  HarvestingManager harvestingManager;
  BuildOrderManager buildOrderManager;
  MorphingUnits morphingUnits;
  ProducingUnits producingUnits;
  BuildingPlaceabilityHelper buildingPlaceabilityHelper;
  FightManager fightManager;
  DelayedExitChecker deleyedExitChecker;
  CostReservation costReservation;
  ProducerManager produceManager;
  LarvaReservations larvaReservations;
  ProductionQueueReservations productionQueueReservations;
  ScoutingManager scoutingManager;
  DiscoveredMemory discoveredMemory;
  BuildTasks buildTasks;
  StrategyExecutor strategyExecutor;
  StrategySelector strategySelector;
  bool umsMap = false;
};

extern BhaalBot* bhaalBot;
