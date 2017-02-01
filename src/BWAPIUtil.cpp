#include <BWAPI.h>
#include <Direction.hpp>

namespace BWAPIUtil
{
  BWAPI::Position tileCenter(BWAPI::TilePosition tilePosition)
  {
    BWAPI::Position result(tilePosition);
    result.x += 16;
    result.y += 16;
    return result;
  }

  void moveByOne(BWAPI::TilePosition* tilePosition, Direction direction)
  {
    switch (direction)
    {
      case Direction::North: --tilePosition->y; break;
      case Direction::NorthEast: --tilePosition->y; ++tilePosition->x; break;
      case Direction::East: ++tilePosition->x; break;
      case Direction::SouthEast: ++tilePosition->y; ++tilePosition->x; break;
      case Direction::South: ++tilePosition->y; break;
      case Direction::SouthWest: ++tilePosition->y; --tilePosition->x; break;
      case Direction::West: --tilePosition->x; break;
      case Direction::NorthWest: --tilePosition->y; --tilePosition->x; break;
    }
  }

}
