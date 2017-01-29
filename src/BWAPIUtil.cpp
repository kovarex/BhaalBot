#include <BWAPI.h>

namespace BWAPIUtil
{
  BWAPI::Position tileCenter(BWAPI::TilePosition tilePosition)
  {
    BWAPI::Position result(tilePosition);
    result.x += 16;
    result.y += 16;
    return result;
  }
}
