#include <Strategy/Strategy.hpp>
#include <Strategy/Strategies.hpp>
#include <BuildOrder.hpp>

Strategy::Strategy(const std::string& name, BWAPI::Race myRace, BWAPI::Race enemyRace, bool isTransition) : name(name)
, myRace(myRace)
, enemyRace(enemyRace)
, isTransition(isTransition)
{
  Strategies::instance().registerStrategy(this);
}

Strategy::~Strategy()
{
  delete this->buildOrder;
}
