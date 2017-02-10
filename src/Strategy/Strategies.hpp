#pragma once
#include <BWAPI.h>
#include <map>
#include <set>
class Strategy;

/** Contains all available strategies and sorts them out by the matchup type. */
class Strategies
{
  Strategies() {}
  ~Strategies();
public:
  void registerStrategy(Strategy* strategy); /**< Adds the strategy to the internal structure so it is available to be picked.
                                              * Ensures that it is deleted at the end. */
  static Strategies& instance()
  {
    static Strategies instance;
    return instance;
  }
  std::vector<Strategy*>& strategiesForMatchup(BWAPI::Race myRace, BWAPI::Race enemyRace);
  Strategy* strategyByName(const std::string name);

private:
  std::map<BWAPI::Race, std::map<BWAPI::Race, std::vector<Strategy*>>> strategiesByMatchup;
  std::vector<Strategy*> strategies;
};
