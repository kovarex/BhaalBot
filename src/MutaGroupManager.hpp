#pragma once
#include <BWAPI.h>
#include <vector>
class Unit;

class MutaGroupManager
{
public:
  MutaGroupManager();
  bool isFull() const { return stackMutas.size() + joiningMutas.size() >= 11; }
  void add(Unit* muta);
  void onFrame();
  void stack();
  double getAverageVelocity();
  double getVelocity(Unit* unit);
  double getStackError();
  BWAPI::Position getCenter();
  void sendJoiningMutas();
  void transferJoiningMutasToStackMutas();
  void attackTarget(BWAPI::Unit unit);
  void chooseClosestTarget();
  void logic();
  void moveStackedMutasWithOverlord(BWAPI::Position position);
  void attackStackedMutasWithOverlord(BWAPI::Unit unit);
  double averageCooldownValue();
  int maxCooldownValue();

  enum class OverallPhase
  {
    Stacking,
    Attacking
  };
  static std::string overallPhaseToString(OverallPhase overallPhase);

  enum class AttackPhase
  {
    Nothing,
    MovingTowardsTarget,
    MovingAway
  };
  static std::string attackPhaseToString(AttackPhase attackPhase);

  std::vector<Unit*> joiningMutas;
  std::vector<Unit*> stackMutas;
  BWAPI::Unit unitTarget = nullptr;
  BWAPI::Position target;
  double angle = 0;
  OverallPhase overalLPhase = OverallPhase::Stacking;
  AttackPhase attackPhase = AttackPhase::Nothing;
  uint32_t ticksSinceAttack = 0;
};
