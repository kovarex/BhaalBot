#include <UnitAsAttackTargetEvaulation.hpp>
#include <Unit.hpp>

double UnitAsAttackTargetEvaluation::valueWithoutTakingHealthIntoConsideration(Unit* unit)
{
  double price = unit->getType().mineralPrice() + unit->getType().gasPrice();
  if (unit->canGather())
    price *= 2;
  return price;
}

double UnitAsAttackTargetEvaluation::value(Unit* unit)
{
  double price = UnitAsAttackTargetEvaluation::valueWithoutTakingHealthIntoConsideration(unit);
  double health = unit->getHitPoints() + unit->getShields();
  if (health < 10)
    health = 10; // to avoid too extreme value for low health units
  return price / health;
}
