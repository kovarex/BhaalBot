#include <UnitSearch.hpp>
#include <Unit.hpp>
#include <BhaalBot.hpp>

UnitSearch::UnitSearch(BWAPI::TilePosition leftTop, BWAPI::TilePosition rightBottom)
  : leftTop(leftTop)
  , rightBottom(rightBottom)
{}

UnitSearch::Iterator::Iterator(BWAPI::TilePosition leftTop, BWAPI::TilePosition rightBottom)
  : currentUnit(nullptr)
  , nextTile(leftTop)
  , leftTop(leftTop)
  , rightBottom(rightBottom)
{}

void UnitSearch::Iterator::operator++()
{
  this->currentUnit = this->currentUnit->nextOnTile;
  this->searchNextUnit();
}

void UnitSearch::Iterator::searchNextUnit()
{
  while (this->currentUnit == nullptr)
  {
    if (this->nextTile.y > this->rightBottom.y)
      return;
    this->currentUnit = bhaalBot->units.getTile(nextTile).units;
    ++this->nextTile.x;
    if (this->nextTile.x > this->rightBottom.x)
    {
      this->nextTile.x = this->leftTop.x;
      ++this->nextTile.y;
    }
  }
}
