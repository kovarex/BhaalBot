#pragma
#include <Module.hpp>
#include <string>
class BuildOrder;

/** Defines generic high level behavior that is defined by code rather than by data.
 * Contains only build orders now, but as it is code based it can control any bot behavior.
 * The strategy can decide to switch to other strategies, so we can share mid/late game strategies for different kind of openings,
 *  and we can also switch to strategies based on our rules (time, enemy units/expansions, kill counts etc.) */
class Strategy : public Module
{
public:
  /** The constructor automatically register the strategy to the Strategies.*/
  Strategy(const std::string& name,
           BWAPI::Race myRace,
           BWAPI::Race enemyRace,
           bool isTransition);
  virtual ~Strategy();
  virtual void onStart() {}

  std::string name; /**< Name of the strategy, must be unique */
  BWAPI::Race myRace; /**< For which race is the strategy meant to be used */
  BWAPI::Race enemyRace; /**< Enemy race, BWAPI::RACES::None meanst that it works for all the races,
                          * BWAPI::RACES::Random means that it is specific for random enemy race */
  bool isTransition = false; /** When @c true this strategy can be used only as mid/late game transition from starting strategy */
  BuildOrder* buildOrder = nullptr; /**< Working build order of the strategy, it is manipulated by the individual
                                     * strategies, it is here only to make sure it is always deleted properly */
  Strategy* switchOrder = nullptr; /**< By setting this to some strategy, it is signaled that this strategy wants to switch to something else. */
};
