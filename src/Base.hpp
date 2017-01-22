#pragma once
#include <BWEM/bwem.h>

class Base
{
public:
  Base(const BWEM::Base* bwemBase) : bwemBase(bwemBase) {}
  int lastCheckedTick = 0;
  bool accessibleOnGround = true;
  BWAPI::Position getCenter() { return bwemBase->Center(); }
  const BWEM::Base* getBWEMBase() { return this->bwemBase; }

  enum class StartingBaseStatus
  {
    None,
    Empty,
    Unknown,
    Enemy
  };
  enum class Status
  {
    Unknown,
    OwnedByMe,
    OwnedByEnemy,
    Empty
  };

  StartingBaseStatus startingBaseStatus = StartingBaseStatus::None;
  Status status = Status::Unknown;
private:
  const BWEM::Base* bwemBase;
};
