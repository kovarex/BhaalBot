#pragma once
#include <Module.hpp>
#include <set>
class Unit;

/** Manages the (our) Unit objects. Provides mapping from the BWAPI::Unit pointer to our unit wrapper. */
class Units : public Module
{
public:
  class TileInfo
  {
  public:
    Unit* units = nullptr;
  };

  Units(ModuleContainer& moduleContainer);
  ~Units();
  void onFrame() override;

  Unit* onUnitComplete(BWAPI::Unit unit);
  void onUnitDestroy(BWAPI::Unit unit);
  Unit* findOrThrow(BWAPI::Unit unit);
  Unit* find(BWAPI::Unit unit);
  void printAssignments();
  void initMap();
  const std::set<Unit*>& getUnits() const { return this->unitSet; }
  TileInfo& getTile(BWAPI::TilePosition position) { return this->map[position.x][position.y]; }

private:
  std::map<BWAPI::Unit, Unit*> units;
  std::set<Unit*> unitSet;
  friend class Unit;
  std::vector<std::vector<TileInfo>> map;
};
