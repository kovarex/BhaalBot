#pragma
#include <Module.hpp>
#include <string>
class BuildOrder;

class Strategy : public Module
{
public:
  Strategy(const std::string& name,
           BWAPI::Race myRace,
           BWAPI::Race enemyRace,
           bool isTransition);
  virtual ~Strategy();
  virtual void onStart() {}


  std::string name;
  BWAPI::Race myRace;
  BWAPI::Race enemyRace;
  bool isTransition = false;
  BuildOrder* buildOrder = nullptr;
  Strategy* switchOrder = nullptr;
};
