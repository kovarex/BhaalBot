#include <Players.hpp>
#include <Player.hpp>
#include <Unit.hpp>
#include <Log.hpp>

Players::Players(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

Players::~Players()
{
  for (Player* player: this->data)
    delete player;
}

void Players::onStart()
{
  for (BWAPI::Player bwapiPlayer: BWAPI::Broodwar->getPlayers())
  {
    Player* player = new Player(bwapiPlayer);
    this->data.push_back(player);
    if (bwapiPlayer == BWAPI::Broodwar->self())
      this->self = player;
    else if (BWAPI::Broodwar->self()->isEnemy(bwapiPlayer))
      this->enemies.push_back(player);
    this->mapping[bwapiPlayer] = player;
  }
}

void Players::onUnitComplete(Unit* unit)
{
  unit->getPlayer()->onUnitComplete(unit);
}

void Players::onUnitDestroy(Unit* unit)
{
  unit->getPlayer()->onUnitDestroy(unit);
}

void Players::onUnitMorph(Unit* unit, BWAPI::UnitType from)
{
  unit->getPlayer()->onUnitMorph(unit, from);
}

void Players::onFrame()
{
  if (this->self)
    this->self->drawDebug(BWAPI::Position(400, 30));
  if (!this->enemies.empty())
    this->enemies[0]->drawDebug(BWAPI::Position(500, 30));
}

Player* Players::findPlayer(BWAPI::Player player)
{
  auto position = this->mapping.find(player);
  if (position == this->mapping.end())
    LOG_AND_ABORT("Couldn't find player");
  return position->second;
}
