#include <Strategy/Strategy.hpp>
#include <Strategy/StrategySelector.hpp>
#include <Strategy/Strategies.hpp>
#include <BhaalBot.hpp>
#include <Log.hpp>

Strategy* StrategySelector::select()
{
  const std::vector<Strategy*>& strategies = Strategies::instance().strategiesForMatchup(BWAPI::Broodwar->self()->getRace(),
                                                                                         BWAPI::Broodwar->enemy()->getRace());
  if (strategies.empty())
    return nullptr;
  if (auto item = bhaalBot->config.findp("preferred-strategy"))
  {
    std::string preferredStrategy = item->get<std::string>().c_str();
    for (Strategy* strategy: strategies)
      if (strategy->name == preferredStrategy)
      {
        LOG_NOTICE("Selected %s based on the config", preferredStrategy.c_str());
        return strategy;
      }
    LOG_WARNING("Didn't find preferred strategy in the config (%s)", preferredStrategy.c_str());
  }
  std::uniform_int_distribution<int> distribution(0, strategies.size() - 1);
  int index = distribution(bhaalBot->randomDevice);
  LOG_NOTICE("Selected %s from %d possible strategies for this matchup", strategies[index]->name, strategies.size());
  return strategies[index];
}
