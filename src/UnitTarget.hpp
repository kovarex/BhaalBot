#pragma once
#include <BWAPI.h>
class Unit;
class UnitMemoryInfo;

/** Pointer to unit that is automatically cleared once the unit object is destroyed. */
class UnitTarget
{
public:
  UnitTarget() {}
  UnitTarget(Unit* unit);
  ~UnitTarget();

  // these can be done later;
  UnitTarget(UnitTarget& other);
  UnitTarget(UnitTarget&& other);
  void operator=(const UnitTarget& other);
  void operator=(Unit* unit);
  Unit* operator->() { return this->unit; }
  Unit* operator*() { return this->unit; }

  bool isZero() const { return this->unit == nullptr; }
  void clear();
  BWAPI::Position getPosition() const;

private:
  friend class Unit;
  Unit* unit = nullptr;
};
