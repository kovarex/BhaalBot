#pragma once
#include <BWAPI.h>
#include <Target.hpp>
class Unit;

class TargetWithPath
{
public:
  bool isZero() const { return this->target.isZero(); }

  std::vector<BWAPI::TilePosition> path; // backwards
  Target target;
};
