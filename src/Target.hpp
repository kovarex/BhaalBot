#pragma once
#include <BWAPI.h>
#include <EnemyUnitTarget.hpp>

class Target
{
public:
  Target() : position(BWAPI::Positions::None) {}
  Target(BWAPI::Position position) : position(position) {}
  Target(BWAPI::Unit unit) : unit(unit) {}
  void operator=(BWAPI::Position position);
  void operator=(BWAPI::Unit unit);
  void operator=(const Target& other) { this->position = other.position; this->unit = other.unit; }

  bool isZero() const;
  BWAPI::Position getPosition() const;
  BWAPI::Unit getUnit() const;
  void clear();

  BWAPI::Position position;
  EnemyUnitTarget unit;
};
