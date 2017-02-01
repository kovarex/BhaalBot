#pragma once
class Unit;

class UnitAsAttackTargetEvaluation
{
public:
  UnitAsAttackTargetEvaluation() = delete;

  static double valueWithoutTakingHealthIntoConsideration(Unit* unit);
  static double value(Unit* unit);
};
