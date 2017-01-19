#include <MutaGroupManager.hpp>
#include <HarvestingManager.hpp>
#include <BhaalBot.hpp>
#include <Unit.hpp>
#include <Vector.hpp>

MutaGroupManager::MutaGroupManager()
 : target(BWAPI::Positions::None)
{}

void MutaGroupManager::add(Unit* muta)
{
  if (this->stackMutas.empty())
    this->stackMutas.push_back(muta);
  else
    this->joiningMutas.push_back(muta);
}

void MutaGroupManager::onFrame()
{
  if (this->target == BWAPI::Positions::None)
    return;
  /*
  HarvestingManager& harvestingManager = KovarexAIModule::instance->harvestingManager;
  if (harvestingManager.bases.empty())
    return;
  BWAPI::Position target = harvestingManager.bases.back().base->getPosition();*/

  /*
  for (BWAPI::Unit muta: this->stackMutas)
    if (muta->isIdle())
      muta->attack(this->target);
  for (BWAPI::Unit muta: this->joiningMutas)
    if (muta->isIdle())
      muta->attack(this->target);*/

  this->logic();

  this->sendJoiningMutas();
  this->transferJoiningMutasToStackMutas();
  BWAPI::Position center = this->getCenter();
  BWAPI::Broodwar->drawTextMap(center.x + 40, center.y - 10, "Velocity: %.2f", this->getAverageVelocity());
  BWAPI::Broodwar->drawTextMap(center.x + 40, center.y + 10, "Stack error: %.2f", this->getStackError());
  BWAPI::Broodwar->drawTextMap(center.x + 40, center.y + 30, "Cooldown: %u / %u", this->maxCooldownValue(), this->countOfMutasWith0Cooldown());
  BWAPI::Broodwar->drawTextMap(center.x + 40, center.y + 50, "State: %s AttackState: %s",
                               MutaGroupManager::overallPhaseToString(this->overalLPhase).c_str(),
                               MutaGroupManager::attackPhaseToString(this->attackPhase).c_str());
  BWAPI::Broodwar->drawTextMap(center.x + 40, center.y + 70, "Stack: %u Joining: %u", this->stackMutas.size(), this->joiningMutas.size());
  
}

void MutaGroupManager::logic()
{
  switch (this->overalLPhase)
  {
  case OverallPhase::Stacking:
    this->stackFast();
    if (this->getStackError() < 1)
      this->overalLPhase = OverallPhase::Attacking;
    break;
  case OverallPhase::Attacking:
    switch (this->attackPhase)
    {
    case AttackPhase::Nothing:
      if (this->getStackError() > 20)
      {
        this->stackingCenter = this->getCenter();
        this->overalLPhase = OverallPhase::Stacking;
        return;
      }
      if (this->unitTarget == nullptr || !this->unitTarget->exists())
        this->chooseClosestTarget();
      if (this->unitTarget != nullptr)
      {
        this->attackPhase = AttackPhase::MovingTowardsTarget;
        this->logic();
      }
      break;
    case AttackPhase::MovingTowardsTarget:
      {
        if (this->unitTarget == nullptr || !this->unitTarget->exists())
        {
          this->unitTarget = nullptr;
          this->attackPhase = AttackPhase::Nothing;
          this->logic();
          return;
        }
        BWAPI::Position center = this->getCenter();
        BWAPI::Position unitPosition = this->unitTarget->getPosition();
        Vector vector(center, unitPosition);
        if (vector.getLength() < 170)
        {
          this->attackStackedMutasWithOverlord(this->unitTarget);
          this->attackPhase = AttackPhase::MovingAway;
          this->ticksSinceAttack = 0;
          return;
        }
        
        vector.extendToLength(200);
        BWAPI::Position behindUnit = unitPosition;
        vector.addTo(&behindUnit);
        this->moveStackedMutasWithOverlord(behindUnit);
      }
      break;
    case AttackPhase::MovingAway:
    {
      ++this->ticksSinceAttack;
      if (this->ticksSinceAttack < 2)
        break;
      BWAPI::Position center = this->getCenter();
      if (ticksSinceAttack < 10)
        this->moveStackedMutasWithOverlord(BWAPI::Position(center.x - 150, center.y));
      else
        this->moveStackedMutasWithOverlord(BWAPI::Position(center.x - 150, center.y - 150));
      if (this->maxCooldownValue() < BWAPI::UnitTypes::Zerg_Mutalisk.groundWeapon().damageCooldown() / 2)
        this->attackPhase = AttackPhase::Nothing;
    }
      break;
    }
    break;
  }
}

BWAPI::Unit MutaGroupManager::getOverlordFarAway(BWAPI::Position position)
{
  BWAPI::Unit overlord = nullptr;
  for (BWAPI::Unit unit: BWAPI::Broodwar->self()->getUnits())
    if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord &&
        unit->getPosition().getDistance(position) > 200)
      return unit;
  return nullptr;
}

void MutaGroupManager::moveStackedMutasWithOverlord(BWAPI::Position position)
{
  BWAPI::Unitset mutaSet;
  for (Unit* muta: this->stackMutas)
    mutaSet.insert(muta->getBWAPIUnit());
  if (mutaSet.empty())
    return;
  BWAPI::Unit overlord = this->getOverlordFarAway((*mutaSet.begin())->getPosition());
  if (overlord == nullptr)
    return;
  mutaSet.insert(overlord);

  BWAPI::Broodwar->issueCommand(mutaSet, BWAPI::UnitCommand::move(nullptr, position));
  overlord->stop();
}

void MutaGroupManager::attackStackedMutasWithOverlord(BWAPI::Unit unit)
{
    BWAPI::Unitset mutaSet;
  for (Unit* muta: this->stackMutas)
    mutaSet.insert(muta->getBWAPIUnit());
  if (mutaSet.empty())
    return;

  BWAPI::Unit overlord = nullptr;
  for (BWAPI::Unit unit: BWAPI::Broodwar->self()->getUnits())
    if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord &&
        unit->getPosition().getDistance((*mutaSet.begin())->getPosition()) > 200)
    {
      overlord = unit;
      break;
    }
  if (overlord == nullptr)
    return;
  mutaSet.insert(overlord);

  BWAPI::Broodwar->issueCommand(mutaSet, BWAPI::UnitCommand::attack(unit, BWAPI::Positions::None));
  overlord->stop();
}

double MutaGroupManager::averageCooldownValue()
{
  double sum = 0;
  for (Unit* unit: this->stackMutas)
    sum += unit->getGroundWeaponCooldown();
  return sum / this->stackMutas.size();
}

int MutaGroupManager::maxCooldownValue()
{
  int result = 0;
  for (Unit* unit: this->stackMutas)
  {
    int cooldown = unit->getGroundWeaponCooldown();
    if (cooldown > result)
      result = cooldown;
  }
  return result;
}

int MutaGroupManager::countOfMutasWith0Cooldown()
{
  int result = 0;
  for (Unit* unit: this->stackMutas)
    if (unit->getGroundWeaponCooldown() == 0)
      ++result;
  return result;
}

std::string MutaGroupManager::overallPhaseToString(OverallPhase overallPhase)
{
  switch (overallPhase)
  {
  case OverallPhase::Stacking: return "Stacking";
  case OverallPhase::Attacking: return "Attacking";
  }
  return "<Unknown>";
}

std::string MutaGroupManager::attackPhaseToString(AttackPhase attackPhase)
{
  switch (attackPhase)
  {
  case AttackPhase::Nothing: return "Nothing";
  case AttackPhase::MovingTowardsTarget: return "Moving towards target";
  case AttackPhase::MovingAway: return "Moving away";
  }
  return "<Unknown>";
}

void MutaGroupManager::stack()
{
  BWAPI::Position position = this->getCenter();
  position.x += int(cos(angle) * 150);
  position.y += int(sin(angle) * 150);
  BWAPI::Broodwar->drawCircleMap(position, 10, BWAPI::Colors::Red);
  if (BWAPI::Broodwar->getFrameCount() % 3 == 0)
  {
    angle += 0.2;
    this->moveStackedMutasWithOverlord(position);
  }
}

void MutaGroupManager::stackFast()
{
  /*if (this->getStackError() < 10)
  {
    this->stack();
    return;
  }*/
  //this->stack();
  //return;
  this->moveStackedMutasWithOverlord(this->stackingCenter);
  /*/
  BWAPI::Unit overlord = this->getOverlordFarAway((*this->stackMutas.begin())->getPosition());
  if (overlord == nullptr)
    return;
  BWAPI::Position center = this->getCenter();
  for (Unit* unit: this->stackMutas)
  {
    Vector toCenter(unit->getPosition(), center);
    toCenter.extendToLength(200);
    BWAPI::Position target(center);
    toCenter.addTo(&target);
    BWAPI::Unitset mutaSet;
    mutaSet.insert(unit->getBWAPIUnit());
    mutaSet.insert(overlord);
    BWAPI::Broodwar->issueCommand(mutaSet, BWAPI::UnitCommand::move(nullptr, this->stackingCenter));
   }
  overlord->stop();*/
}

void MutaGroupManager::attackTarget(BWAPI::Unit unit)
{
  this->unitTarget = unit;
}

void MutaGroupManager::chooseClosestTarget()
{
  BWAPI::Position center = this->getCenter();
  BWAPI::Unit candidate = nullptr;
  uint32_t candidateDistance = 0;
  for (BWAPI::Unit unit: BWAPI::Broodwar->getAllUnits())
    if (BWAPI::Broodwar->self()->isEnemy(unit->getPlayer()))
    {
      uint32_t unitDistance = unit->getDistance(center);
      if (candidate == nullptr || unitDistance < candidateDistance)
      {
        candidate = unit;
        candidateDistance = unitDistance;
      }
    }
  this->unitTarget = candidate;
}

double MutaGroupManager::getAverageVelocity()
{
  double velocitySum = 0;
  uint32_t breaking = 0;
  for (Unit* unit: this->stackMutas)
    velocitySum += getVelocity(unit);
  return velocitySum / stackMutas.size();
}

double MutaGroupManager::getVelocity(Unit* unit)
{
  return sqrt(unit->getVelocityX()*unit->getVelocityX() + unit->getVelocityY()*unit->getVelocityY());
}

double MutaGroupManager::getStackError()
{
  BWAPI::Position center = this->getCenter();
  double distanceFromCenterSum = 0;
  for (Unit* unit: this->stackMutas)
    distanceFromCenterSum += unit->getPosition().getDistance(center);
  return distanceFromCenterSum / this->stackMutas.size();
}

BWAPI::Position MutaGroupManager::getCenter()
{
  BWAPI::Position center;
  for (Unit* unit: this->stackMutas)
    center += unit->getPosition();
  center.x /= this->stackMutas.size();
  center.y /= this->stackMutas.size();
  return center;
}

void MutaGroupManager::sendJoiningMutas()
{
  BWAPI::Position center = this->getCenter();
  for (Unit* unit: this->joiningMutas)
    unit->move(center);
}

void MutaGroupManager::transferJoiningMutasToStackMutas()
{
  BWAPI::Position center = this->getCenter();
  for (uint32_t i = 0; i < this->joiningMutas.size(); ++i)
  {
    Unit* muta = this->joiningMutas[i];
    if (muta->getDistance(center) < 50)
    {
      this->joiningMutas.erase(this->joiningMutas.begin() + i);
      this->stackMutas.push_back(muta);
    }
    else
      ++i;
  }
}
