#pragma once
#include <BWEM/bwem.h>
class BaseHarvestingController;
class BaseInDangerDetector;
class TaskForce;

/** Position where mining base can be built. */
class Base
{
public:
  Base(const BWEM::Base* bwemBase) : bwemBase(bwemBase) {}
  int lastCheckedTick = 0;
  bool accessibleOnGround = true;
  BWAPI::Position getCenter() { return bwemBase->Center(); }
  const BWEM::Base* getBWEMBase() { return this->bwemBase; }
  void onFrame();

  enum class Status
  {
    Unknown,
    OwnedByMe,
    OwnedByEnemy,
    Empty
  };
  enum class DefenseState
  {
    None,
    Defending
  };

  enum class DefenseStrategy
  {
    None,
    FightWithDrones,
    EscapeWithDrones
  };

  DefenseState defenseState = DefenseState::None;
  DefenseStrategy defenseStrategy = DefenseStrategy::FightWithDrones;
  Status status = Status::Unknown;
  BaseHarvestingController* harvestingController = nullptr;
  BaseInDangerDetector* baseInDangerDetector = nullptr;
  TaskForce* defendForce = nullptr;
private:
  const BWEM::Base* bwemBase;
};
