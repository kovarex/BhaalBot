#include <LingGroupController.hpp>
#include <BhaalBot.hpp>
#include <Unit.hpp>
#include <Group.hpp>
#include <Vector.hpp>

LingGroupController::LingGroupController(Group& owner)
  : GroupController(owner)
  , targetPosition(BWAPI::Positions::None)
  , targetUnit(nullptr)
{}

void LingGroupController::setAttackTarget(BWAPI::Unit target)
{
  this->setTargetUnit(target);
}

void LingGroupController::onAdded(Unit* muta)
{
  // TODO assign it target. Or is it done in on Frame?
}


void LingGroupController::onFrame()
{
  // TODO
  // if attack frame, let it be
  // - update target of the group
  // - go through lings and update their action
  // -- either attak unit or move to unit or move away

  // the attack target may actually be assigned by taskforce. But perhaps just on the level of pool/drones/...
}


//void LingGroupController::attackTarget(BWAPI::Unit unit)
//{
//  this->setTargetUnit(unit);
//  //this->attack();
//}

void LingGroupController::chooseTarget()
{
  // Unitset 	getUnitsInWeaponRange (WeaponType weapon, const UnitFilter &pred=nullptr) const
  /*
  Unitset BWAPI::UnitInterface::getUnitsInRadius(int 	radius,
    const UnitFilter & 	pred = nullptr
  )		const 
  http://bwapi.github.io/class_b_w_a_p_i_1_1_unit_interface.html#ae4c5496a45e056afde8429295f5d54f7
  */
  // virtual bool 	isAttackFrame () const

  // TODO
  /*
    - repeatedly get units in some distance (radius)
    priority: 
      1 hit kill in range
      if almost dead, flee
      2 hit kill in range 
      ling in range, 
      drone in range;
      nearest ling with < 2 attackers
      separated drone
      any drone
  */

}

BWAPI::Position LingGroupController::getCenter()
{
  if (this->owner.getUnits().empty())
    return BWAPI::Positions::None;
  BWAPI::Position center;
  for (Unit* unit : this->owner.getUnits())
    center += unit->getPosition();
  center.x /= this->owner.getUnits().size();
  center.y /= this->owner.getUnits().size();
  return center;
}