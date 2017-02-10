#include <Strategy/Strategies.hpp>
#include <Strategy/Strategy.hpp>

Strategies::~Strategies()
{
  for (Strategy* strategy: this->strategies)
    delete strategy;
}

void Strategies::registerStrategy(Strategy* strategy)
{
  this->strategies.push_back(strategy);
  if (strategy->enemyRace == BWAPI::Races::None)
  {
    this->strategiesByMatchup[strategy->myRace][BWAPI::Races::Zerg].push_back(strategy);
    this->strategiesByMatchup[strategy->myRace][BWAPI::Races::Protoss].push_back(strategy);
    this->strategiesByMatchup[strategy->myRace][BWAPI::Races::Terran].push_back(strategy);
    this->strategiesByMatchup[strategy->myRace][BWAPI::Races::Unknown].push_back(strategy);
  }
  else
    this->strategiesByMatchup[strategy->myRace][strategy->enemyRace].push_back(strategy);
}

std::vector<Strategy*>& Strategies::strategiesForMatchup(BWAPI::Race myRace, BWAPI::Race enemyRace)
{
  return this->strategiesByMatchup[myRace][enemyRace];
}

Strategy* Strategies::strategyByName(const std::string name)
{
  for (Strategy* strategy: this->strategies)
    if (strategy->name == name)
      return strategy;
  return nullptr;
}
