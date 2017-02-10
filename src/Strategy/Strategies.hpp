#pragma once
#include <BWAPI.h>
#include <map>
#include <set>
class Strategy;

class Strategies
{
  Strategies() {}
  ~Strategies();
public:
  void registerStrategy(Strategy* strategy);

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
