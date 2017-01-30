#pragma once
#include <Module.hpp>
class Player;

class Players : Module
{
public:
  Players(ModuleContainer& moduleContainer);
  ~Players();
  void onStart() override;
  void onUnitComplete(Unit* unit) override;
  void onUnitDestroy(Unit* unit) override;
  void onUnitMorph(Unit* unit, BWAPI::UnitType from) override;
  void onFrame() override;
  Player* findPlayer(BWAPI::Player player);
  
  std::vector<Player*> data;
  std::map<BWAPI::Player, Player*> mapping;
  std::vector<Player*> enemies;
  Player* self = nullptr;
};
