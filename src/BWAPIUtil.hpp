#pragma once
#include <BWAPI.h>
class Direction;

namespace BWAPIUtil
{
  BWAPI::Position tileCenter(BWAPI::TilePosition tilePosition);
  void moveByOne(BWAPI::TilePosition* tilePosition, Direction direction);
}
