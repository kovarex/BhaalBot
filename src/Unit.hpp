#pragma once
#include <BWAPI.h>
#include <Target.hpp>
class Assignment;
class Player;

/**< Wrapper of the BWAPI::Unit with our additional info.
 * It is currently used only for our units. */
class Unit
{
public:
  Unit(BWAPI::Unit bwapiUnit);
  ~Unit();

  /* The BWAPI::Unit interface is re-routed to the BWAPI::Unit in these methods. */
  BWAPI::Player getBWAPIPlayer() { return this->bwapiUnit->getPlayer(); }
  Player* getPlayer() { return this->player; }
  BWAPI::UnitType getType() const { return this->bwapiUnit->getType(); }
  BWAPI::UnitType getBuildType() const { return this->bwapiUnit->getBuildType(); }
  BWAPI::Position getPosition() const { return this->bwapiUnit->getPosition(); }
  bool isCarryingMinerals() const { return this->bwapiUnit->isCarryingMinerals(); }
  bool isGatheringMinerals() const { return this->bwapiUnit->isGatheringMinerals(); }
  BWAPI::Unit getOrderTarget() { return this->bwapiUnit->getOrderTarget(); }
  bool canGather() { return this->bwapiUnit->canGather(); }
  void rightClick(BWAPI::Unit unit) { this->bwapiUnit->rightClick(unit); }
  void gather(BWAPI::Unit unit) { this->bwapiUnit->gather(unit); }
  bool isIdle() { return this->bwapiUnit->isIdle(); }
  bool isGatheringGas() { return this->bwapiUnit->isGatheringGas(); }
  bool isVisible() { return this->bwapiUnit->isVisible(); }
  BWAPI::Unit getTarget() { return this->bwapiUnit->getTarget(); }
  int getDistance(BWAPI::Unit unit) const { return this->bwapiUnit->getDistance(unit); }
  int getDistance(BWAPI::Position position) const { return this->bwapiUnit->getDistance(position); }
  BWAPI::Unitset getLarva() { return this->bwapiUnit->getLarva(); }
  BWAPI::Unitset getUnitsInRadius(int radius, const BWAPI::UnitFilter &pred = nullptr) const { return this->bwapiUnit->getUnitsInRadius(radius, pred); }
  BWAPI::Unit getClosestUnit(const BWAPI::UnitFilter &pred = nullptr, int radius = 999999) const { return this->bwapiUnit->getClosestUnit(pred, radius); }
  BWAPI::Unitset getUnitsInWeaponRange(BWAPI::WeaponType weapon, const BWAPI::UnitFilter &pred = nullptr) const { return this->bwapiUnit->getUnitsInWeaponRange(weapon, pred); }
  bool isInWeaponRange(BWAPI::Unit unit) const { return this->bwapiUnit->isInWeaponRange(unit); }
  bool morph(BWAPI::UnitType unitType) { return this->bwapiUnit->morph(unitType); }
  bool canAttack() const { return this->bwapiUnit->canAttack(); }
  bool canMove() const { return this->bwapiUnit->canMove(); }
  bool canTrain(BWAPI::UnitType unitType) const { return this->bwapiUnit->canTrain(unitType); }
  bool train(BWAPI::UnitType unitType) { return this->bwapiUnit->train(unitType); }
  BWAPI::Unit getHatchery() { return this->bwapiUnit->getHatchery(); }
  double getVelocityX() const { return this->bwapiUnit->getVelocityX(); }
  double getVelocityY() const { return this->bwapiUnit->getVelocityY(); }
  double getHitPoints() const { return this->bwapiUnit->getHitPoints(); }
  double getShields() const { return this->bwapiUnit->getShields(); }
  bool build(BWAPI::UnitType unitType, BWAPI::TilePosition position) { return this->bwapiUnit->build(unitType, position); }
  BWAPI::Order getOrder() { return this->bwapiUnit->getOrder(); }
  bool move(BWAPI::Position position) { return this->bwapiUnit->move(position); }
  BWAPI::Position getTargetPosition() { return this->bwapiUnit->getTargetPosition(); }
  int getGroundWeaponCooldown() const { return this->bwapiUnit->getGroundWeaponCooldown(); }
  bool isConstructing() const { return this->bwapiUnit->isConstructing(); }
  bool attack(BWAPI::Unit unit) { return this->bwapiUnit->attack(unit); }
  bool attack(BWAPI::Position position) { return this->bwapiUnit->attack(position); }
  bool isAttackFrame() const { return this->bwapiUnit->isAttackFrame();  }
  int getID() const { return this->bwapiUnit->getID(); }
  int getRemainingBuildTime() const { return this->bwapiUnit->getRemainingBuildTime(); }

  bool attack(Unit* unit) { return this->bwapiUnit->attack(unit->getBWAPIUnit()); }
  void printAssignment();
  int getDistance(Unit* unit) const { return this->bwapiUnit->getDistance(unit->bwapiUnit); }
  bool equals(BWAPI::Unit unit) { return this->bwapiUnit == unit; }
  BWAPI::Unit getBWAPIUnit() { return this->bwapiUnit; }
  static std::string shortenUnitName(const std::string& name); /**< Removes the rdundant Zerg_, Protoss_, Terran_ prefixes. */
  std::string getName() const; 

  void addTarget(UnitTarget* target);
  void removeTarget(UnitTarget* target);

  /** Connects the assignment with this unit. If there is some assignment already it is properly removed.
   * @param assignment nullptr only unassigns the current assignment. */
  void assign(Assignment* assignment);
  Assignment* getAssignment() { return this->assignment; }
  void updatePosition();

private:
  void registerOnMap();
  void unregsiterFromMap();

  BWAPI::Unit bwapiUnit;
  Assignment* assignment = nullptr;
public:
  class Order
  {
  public:
    enum class Type
    {
      None,
      Attack
    };
    Order() {}
    Order(Type type) : type(type) {}
    Order(Type type, Unit* target) : type(type), target(target) {}
    Order(Order& other) = default;
    void operator=(const Order& other) { this->type = other.type; this->target = other.target; }

    Type type = Type::None;
    Target target;
  };
  Order lastOrderGiven;
  int frameOfLastOrder = 0;
  std::vector<UnitTarget*> targetingMe;
  UnitMemoryInfo* memoryInfo = nullptr; // only used for foreign units
  BWAPI::UnitType lastSeenUnitType;
  BWAPI::Position lastSeenPosition;
  Player* player = nullptr;
  Unit* previousOnTile = nullptr;
  Unit* nextOnTile = nullptr;
#ifdef DEBUG
  std::string name; /** For easy unit evaulation while debugging. */
#endif
};
