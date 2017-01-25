#pragma once
#include <BWAPI.h>
class UnitMemoryInfo;

class EnemyUnitTarget
{
public:
  EnemyUnitTarget() {}
  EnemyUnitTarget(BWAPI::Unit unit);
  ~EnemyUnitTarget();

  // these can be done later;
  EnemyUnitTarget(EnemyUnitTarget& other)  = delete;
  EnemyUnitTarget(EnemyUnitTarget&& other)  = delete;
  void operator=(EnemyUnitTarget& other) = delete;
  void operator=(BWAPI::Unit unit);


  bool isZero() const { return this->data == nullptr; }
  void clear();
  UnitMemoryInfo* getUnitData() { return this->data; }
  BWAPI::Position getPosition() const;
  BWAPI::Unit getUnit() const;

private:
  friend class UnitMemoryInfo;
  UnitMemoryInfo* data = nullptr;
};
