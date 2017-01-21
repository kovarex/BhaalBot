#pragma once
#include <BWAPI.h>
class Assignment;

/**< Wrapper of the BWAPI::Unit with our additional info.
 * It is currently used only for our units. */
class Unit
{
public:
  Unit(BWAPI::Unit bwapiUnit) : bwapiUnit(bwapiUnit) {}
  ~Unit();

  /* The BWAPI::Unit interface is re-routed to the BWAPI::Unit in these methods. */
  BWAPI::Player getPlayer() { return this->bwapiUnit->getPlayer(); }
  BWAPI::UnitType getType() { return this->bwapiUnit->getType(); }
  BWAPI::UnitType getBuildType() { return this->bwapiUnit->getBuildType(); }
  BWAPI::Position getPosition() { return this->bwapiUnit->getPosition(); }
  bool isCarryingMinerals() { return this->bwapiUnit->isCarryingMinerals(); }
  bool isGatheringMinerals() { return this->bwapiUnit->isGatheringMinerals(); }
  BWAPI::Unit getOrderTarget() { return this->bwapiUnit->getOrderTarget(); }
  bool canGather() { return this->bwapiUnit->canGather(); }
  void rightClick(BWAPI::Unit unit) { this->bwapiUnit->rightClick(unit); }
  void gather(BWAPI::Unit unit) { this->bwapiUnit->gather(unit); }
  bool isIdle() { return this->bwapiUnit->isIdle(); }
  bool isGatheringGas() { return this->bwapiUnit->isGatheringGas(); }
  BWAPI::Unit getTarget() { return this->bwapiUnit->getTarget(); }
  int getDistance(BWAPI::Unit unit) const { return this->bwapiUnit->getDistance(unit); }
  int getDistance(BWAPI::Position position) const { return this->bwapiUnit->getDistance(position); }
  BWAPI::Unitset getLarva() { return this->bwapiUnit->getLarva(); }
  BWAPI::Unitset getUnitsInRadius(int radius) { return this->bwapiUnit->getUnitsInRadius(radius); }
  bool morph(BWAPI::UnitType unitType) { return this->bwapiUnit->morph(unitType); }
  bool canAttack() const { return this->bwapiUnit->canAttack(); }
  bool canTrain(BWAPI::UnitType unitType) const { return this->bwapiUnit->canTrain(unitType); }
  bool train(BWAPI::UnitType unitType) { return this->bwapiUnit->train(unitType); }
  BWAPI::Unit getHatchery() { return this->bwapiUnit->getHatchery(); }
  double getVelocityX() const { return this->bwapiUnit->getVelocityX(); }
  double getVelocityY() const { return this->bwapiUnit->getVelocityY(); }
  bool build(BWAPI::UnitType unitType, BWAPI::TilePosition position) { return this->bwapiUnit->build(unitType, position); }
  BWAPI::Order getOrder() { return this->bwapiUnit->getOrder(); }
  bool move(BWAPI::Position position) { return this->bwapiUnit->move(position); }
  BWAPI::Position getTargetPosition() { return this->bwapiUnit->getTargetPosition(); }
  int getGroundWeaponCooldown() const { return this->bwapiUnit->getGroundWeaponCooldown(); }
  bool isConstructing() const { return this->bwapiUnit->isConstructing(); }

  void printAssignment();
  int getDistance(Unit* unit) const { return this->bwapiUnit->getDistance(unit->bwapiUnit); }
  bool equals(BWAPI::Unit unit) { return this->bwapiUnit == unit; }
  BWAPI::Unit getBWAPIUnit() { return this->bwapiUnit; }

  /** Connects the assignment with this unit. If there is some assignment already it is properly removed.
   * @param assignment nullptr only unassigns the current assignment. */
  void assign(Assignment* assignment);

private:
  BWAPI::Unit bwapiUnit;
  Assignment* assignment = nullptr;
};
