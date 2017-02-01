#pragma once
class Unit;

class UnitAsAttackTargetEvaluation
{
  UnitAsAttackTargetEvaluation() = delete;

  double valueWithoutTakingHealthIntoConsideration(Unit* unit);
  double value(Unit* unit);
};
