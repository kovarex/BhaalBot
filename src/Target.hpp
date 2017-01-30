#pragma once
#include <BWAPI.h>
#include <UnitTarget.hpp>

class Target
{
public:
  Target() : position(BWAPI::Positions::None) {}
  Target(BWAPI::Position position) : position(position) {}
  Target(Unit* unit) : unit(unit) {}
  void operator=(BWAPI::Position position);
  void operator=(Unit* unit);
  void operator=(const Target& other) { this->position = other.position; this->unit = other.unit; }

  bool isZero() const;
  BWAPI::Position getPosition() const;
  Unit* getUnit();
  void clear();

  BWAPI::Position position;
  UnitTarget unit;
};
