#pragma once
#include <BWAPI.h>
#include <BuildLocationType.hpp>
#include <Cost.hpp>
class BhaalBot;
class UnitCompositionSelector;

class BuildOrderItem
{
public:
  virtual ~BuildOrderItem() {}
  virtual bool execute() = 0;
  virtual std::string str() const = 0;
  virtual int getPlannedType(BWAPI::UnitType unitType) const { return 0; } // TODO change function name into something understandable
  virtual Cost getCost() const { return Cost(); }
};

class BuildBuildOrderItem : public BuildOrderItem
{
public:
  BuildBuildOrderItem(BWAPI::UnitType unit, BuildLocationType location) : unit(unit), location(location) {}
  bool execute() override;
  std::string str() const override;
  int getPlannedType(BWAPI::UnitType unitType) const override { return unit == unitType ? 1 : 0; }
  Cost getCost() const override;

  BuildLocationType location = BuildLocationType::Auto;
  BWAPI::UnitType unit;
};

class SwitchToAutomaticSupplyBuilding : public BuildOrderItem
{
public:
  bool execute() override;
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

class SetUnitComposition : public BuildOrderItem
{
public:
  SetUnitComposition(UnitCompositionSelector* unitCompositionSelector)
    : unitCompositionSelector(unitCompositionSelector) {}
  virtual ~SetUnitComposition();
  std::string str() const override { return "Set unit composition"; }
  bool execute() override;

  UnitCompositionSelector* unitCompositionSelector;
};
