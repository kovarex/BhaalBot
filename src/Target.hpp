#pragma once
#include <BWAPI.h>
#include <UnitTarget.hpp>

/** Targets unit or position. */
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
  BWAPI::Position getPosition() const; /**< If the unit is valid returns position of the unit, otherwise, it returns position. */
  Unit* getUnit();
  void clear();

  BWAPI::Position position;
  UnitTarget unit;
};
