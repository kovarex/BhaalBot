#pragma once
#include <BWAPI.h>
#include <BuildLocationType.hpp>
class BhaalBot;

class BuildOrderItem
{
public:
  virtual bool execute() = 0;
  virtual std::string str() const = 0;
};

class BuildBuildOrderItem : public BuildOrderItem
{
public:
  BuildBuildOrderItem(BWAPI::UnitType unit, BuildLocationType location) : unit(unit), location(location) {}
  virtual bool execute();
  virtual std::string str() const override;

  BuildLocationType location = BuildLocationType::Auto;
  BWAPI::UnitType unit;
};

class SwitchToAutomaticOverlordBuilding : public BuildOrderItem
{
public:
  virtual bool execute();
  virtual std::string str() const override { return "Switch to automatic overlord building"; }
};

class SendScoutBuildOrderItem : public BuildOrderItem
{
public:
  SendScoutBuildOrderItem(BWAPI::UnitType unitType) : unitType(unitType) {}
  virtual std::string str() const override { return "Send scout."; }
  bool execute() override;

  BWAPI::UnitType unitType;
};
