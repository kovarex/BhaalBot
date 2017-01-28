#include <Base.hpp>
#include <BaseHarvestingController.hpp>
#include <DroneDefenseGroupController.hpp>
#include <Group.hpp>
#include <Unit.hpp>

DroneDefenseGroupController::DroneDefenseGroupController(Group& owner, Base* base)
  : GroupController(owner)
  , base(base)
{
 
}

void DroneDefenseGroupController::onAdded(Unit* unit)
{
  this->goingToMineral.insert(unit);
}

void DroneDefenseGroupController::onFrame()
{
 if (this->defenseMineral == nullptr &&
     base->harvestingController &&
    !base->harvestingController->minerals.empty())
   this->defenseMineral = base->harvestingController->minerals[0]->mineral;
  if (this->defenseMineral)
    for (Unit* unit: this->owner.getUnits())
      unit->gather(this->defenseMineral);
}
