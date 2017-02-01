#pragma once
#include <BWAPI.h>
class Unit;

class UnitSearch
{
public:
  UnitSearch(BWAPI::TilePosition leftTop, BWAPI::TilePosition rightBottom);

  class Iterator
  {
  public:
    Iterator(BWAPI::TilePosition leftTop, BWAPI::TilePosition rightBottom);
    Iterator() : currentUnit(nullptr), nextTile(BWAPI::Positions::None) {}
    Unit* operator*() { return this->currentUnit; }
    Unit* operator->() { return this->currentUnit; }
    void operator++();
    bool operator==(const Iterator& other) const { return this->currentUnit == other.currentUnit; }

  private:
    void searchNextUnit();

    Unit* currentUnit;
    BWAPI::TilePosition nextTile;
    BWAPI::TilePosition leftTop;
    BWAPI::TilePosition rightBottom;
  };

  Iterator begin() const { return Iterator(this->leftTop, this->rightBottom); }
  Iterator end() const { return Iterator(); }

  BWAPI::TilePosition leftTop, rightBottom;
};
