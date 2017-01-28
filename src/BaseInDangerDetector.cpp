#include <Base.hpp>
#include <BaseInDangerDetector.hpp>
#include <BWAPI.h>
#include <Log.hpp>

BaseInDangerDetector::BaseInDangerDetector(Base& base)
  : base(base)
{
  if (this->base.baseInDangerDetector != nullptr)
    LOG_AND_ABORT("this->base.baseInDangerDetector != nullptr");
  base.baseInDangerDetector = this;
}

BaseInDangerDetector::~BaseInDangerDetector()
{
  if (this->base.baseInDangerDetector != this)
    LOG_AND_ABORT("this->base.baseInDdangerDetector != this");
  base.baseInDangerDetector = nullptr;
}

void BaseInDangerDetector::onFrame()
{
  this->dangerLevel = 0;
  BWAPI::Unitset unitsAroundBase = BWAPI::Broodwar->getUnitsInRadius(this->base.getCenter(), 500);
  for (BWAPI::Unit unit: unitsAroundBase)
  {
    LOG_INFO("Checking %s isEnemy: %s canAttack: %s",
             unit->getType().getName().c_str(),
             BWAPI::Broodwar->self()->isEnemy(unit->getPlayer()) ? "true" : "false",
             unit->canAttack() ? "true" : "false");
    if (BWAPI::Broodwar->self()->isEnemy(unit->getPlayer()) &&
        unit->canAttack(false))
    {
      if (unit->canGather())
        ++dangerLevel;
      else
        dangerLevel += 2;
    }
  }
}
