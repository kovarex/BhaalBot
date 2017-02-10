#pragma once
#include <BWAPI.h>
#include <BuildLocationType.hpp>
class BhaalBot;

class BuildOrderItem
{
public:
  virtual bool execute() = 0;
  virtual std::string str() const = 0;
  virtual int getPlannedType(BWAPI::UnitType unitType) const { return 0; }
};

class BuildBuildOrderItem : public BuildOrderItem
{
public:
  BuildBuildOrderItem(BWAPI::UnitType unit, BuildLocationType location) : unit(unit), location(location) {}
  bool execute();
  std::string str() const override;
  int getPlannedType(BWAPI::UnitType unitType) const override { return unit == unitType ? 1 : 0; }

  BuildLocationType location = BuildLocationType::Auto;
  BWAPI::UnitType unit;
};

class SwitchToAutomaticOverlordBuilding : public BuildOrderItem
{
public:
  bool execute();
  std::string str() const override { return "Switch to automatic overlord building"; }
};

class SendScoutBuildOrderItem : public BuildOrderItem
{
public:
  SendScoutBuildOrderItem(BWAPI::UnitType unitType) : unitType(unitType) {}
  std::string str() const override { return "Send scout."; }
  bool execute() override;

  BWAPI::UnitType unitType;
};
