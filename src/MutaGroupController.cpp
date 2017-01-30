#include <MutaGroupController.hpp>
#include <HarvestingManager.hpp>
#include <BhaalBot.hpp>
#include <Player.hpp>
#include <Unit.hpp>
#include <Vector.hpp>

MutaGroupController::MutaGroupController(Group& owner)
 : GroupController(owner)
{}

void MutaGroupController::onAdded(Unit* muta)
{
  if (this->stackMutas.empty())
    this->stackMutas.push_back(muta);
  else
    this->joiningMutas.push_back(muta);
}

void MutaGroupController::onRemoved(Unit* unit)
{
  for (auto it = this->stackMutas.begin(); it != this->stackMutas.end(); ++it)
    if (*it == unit)
    {
      this->stackMutas.erase(it);
      return;
    }
  for (auto it = this->joiningMutas.begin(); it != this->joiningMutas.end(); ++it)
    if (*it == unit)
    {
      this->joiningMutas.erase(it);
      return;
    }
}

void MutaGroupController::onFrame()
{
  if (this->target.isZero())
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
                               MutaGroupController::overallPhaseToString(this->overalLPhase).c_str(),
                               MutaGroupController::attackPhaseToString(this->attackPhase).c_str());
  BWAPI::Broodwar->drawTextMap(center.x + 40, center.y + 70, "Stack: %u Joining: %u", this->stackMutas.size(), this->joiningMutas.size());
  
}

void MutaGroupController::logic()
{
  switch (this->overalLPhase)
  {
  case OverallPhase::StackingPhase1:
    this->stack(false);
    if (this->getStackError() < 5)
      this->overalLPhase = OverallPhase::StackingPhase2;
    break;
  case OverallPhase::StackingPhase2:
    this->stack(true);
    if (this->getAverageVelocity() > 6.5)
      this->overalLPhase = OverallPhase::Attacking;
    break;
  case OverallPhase::Attacking:
    switch (this->attackPhase)
    {
    case AttackPhase::Nothing:
      if (this->getStackError() > 15)
      {
        this->overalLPhase = OverallPhase::StackingPhase1;
        return;
      }
      if (this->target.isZero())
        this->chooseClosestTarget();
      if (!this->target.isZero())
      {
        this->attackPhase = AttackPhase::MovingTowardsTarget;
        this->logic();
      }
      break;
    case AttackPhase::MovingTowardsTarget:
      {
        if (this->target.isZero())
        {
          this->target.clear();
          this->attackPhase = AttackPhase::Nothing;
          this->logic();
          return;
        }
        BWAPI::Position center = this->getCenter();
        BWAPI::Position unitPosition = this->target.getPosition();
        Vector originalVector(center, unitPosition);
        Vector targetVector(originalVector);
        targetVector.extendToLength(250);
        BWAPI::Position behindUnit = center;
        targetVector.addTo(&behindUnit);

        if (originalVector.getLength() < 170 && this->maxCooldownValue() == 0)
        {
          this->attackStackedMutasWithOverlord(this->target.getUnit(), BWAPI::Positions::None);
          BWAPI::Broodwar->drawCircleMap(behindUnit, 10, BWAPI::Colors::Red);
          this->attackPhase = AttackPhase::MovingAway;
          this->ticksSinceAttack = 0;
          return;
        }
        
        this->moveStackedMutasWithOverlord(behindUnit);
        BWAPI::Broodwar->drawCircleMap(behindUnit, 10, BWAPI::Colors::Blue);
      }
      break;
    case AttackPhase::MovingAway:
    {
      ++this->ticksSinceAttack;
      if (this->ticksSinceAttack < 2)
        break;
      BWAPI::Position center = this->getCenter();
      BWAPI::Position target;
      if (ticksSinceAttack < 10)
        target = BWAPI::Position(center.x - 100, center.y);
      else
        target = BWAPI::Position(center.x - 100, center.y - 100);
      this->moveStackedMutasWithOverlord(target);
      BWAPI::Broodwar->drawCircleMap(target, 10, BWAPI::Colors::Blue);
      if (this->maxCooldownValue() < BWAPI::UnitTypes::Zerg_Mutalisk.groundWeapon().damageCooldown() / 2 &&
          (this->target.isZero() || this->target.getPosition().getDistance(center) > 150))
        this->attackPhase = AttackPhase::Nothing;
    }
      break;
    }
    break;
  }
}

BWAPI::Unit MutaGroupController::getOverlordFarAway(BWAPI::Position position)
{
  BWAPI::Unit overlord = nullptr;
  for (BWAPI::Unit unit: BWAPI::Broodwar->self()->getUnits())
    if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord &&
        unit->getPosition().getDistance(position) > 200)
      return unit;
  return nullptr;
}

void MutaGroupController::moveStackedMutasWithOverlord(BWAPI::Position position)
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

void MutaGroupController::attackStackedMutasWithOverlord(Unit* unit, BWAPI::Position position)
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

  BWAPI::Broodwar->issueCommand(mutaSet, BWAPI::UnitCommand::attack(unit->getBWAPIUnit(), position));
  overlord->stop();
}

double MutaGroupController::averageCooldownValue()
{
  double sum = 0;
  for (Unit* unit: this->stackMutas)
    sum += unit->getGroundWeaponCooldown();
  return sum / this->stackMutas.size();
}

int MutaGroupController::maxCooldownValue()
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

int MutaGroupController::countOfMutasWith0Cooldown()
{
  int result = 0;
  for (Unit* unit: this->stackMutas)
    if (unit->getGroundWeaponCooldown() == 0)
      ++result;
  return result;
}

std::string MutaGroupController::overallPhaseToString(OverallPhase overallPhase)
{
  switch (overallPhase)
  {
  case OverallPhase::StackingPhase1: return "StackingPhase1";
  case OverallPhase::StackingPhase2: return "StackingPhase2";
  case OverallPhase::Attacking: return "Attacking";
  }
  return "<Unknown>";
}

std::string MutaGroupController::attackPhaseToString(AttackPhase attackPhase)
{
  switch (attackPhase)
  {
  case AttackPhase::Nothing: return "Nothing";
  case AttackPhase::MovingTowardsTarget: return "Moving towards target";
  case AttackPhase::MovingAway: return "Moving away";
  }
  return "<Unknown>";
}

void MutaGroupController::stack(bool secondPhase)
{
  BWAPI::Position position = this->getCenter();
  double distance = secondPhase ? 150 : 40;
  position.x += int(cos(angle) * distance);
  position.y += int(sin(angle) * distance);
  BWAPI::Broodwar->drawCircleMap(position, 10, BWAPI::Colors::Blue);
  if (BWAPI::Broodwar->getFrameCount() % 2 == 0)
  {
    angle += 0.5;
    this->moveStackedMutasWithOverlord(position);
  }
}

void MutaGroupController::stackFast()
{
  this->stack(true);
  return;
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

void MutaGroupController::chooseClosestTarget()
{
  BWAPI::Position center = this->getCenter();
  Unit* candidate = nullptr;
  uint32_t candidateDistance = 0;
  for (Player* player: bhaalBot->players.enemies)
    for (Unit* unit: player->units)
      {
        uint32_t unitDistance = unit->getDistance(center);
        if (candidate == nullptr || unitDistance < candidateDistance)
        {
          candidate = unit;
          candidateDistance = unitDistance;
        }
      }
  this->target = candidate;
}

double MutaGroupController::getAverageVelocity()
{
  double velocitySum = 0;
  uint32_t breaking = 0;
  for (Unit* unit: this->stackMutas)
    velocitySum += getVelocity(unit);
  return velocitySum / stackMutas.size();
}

double MutaGroupController::getVelocity(Unit* unit)
{
  return sqrt(unit->getVelocityX()*unit->getVelocityX() + unit->getVelocityY()*unit->getVelocityY());
}

double MutaGroupController::getStackError()
{
  BWAPI::Position center = this->getCenter();
  double distanceFromCenterSum = 0;
  for (Unit* unit: this->stackMutas)
    distanceFromCenterSum += unit->getPosition().getDistance(center);
  return distanceFromCenterSum / this->stackMutas.size();
}

BWAPI::Position MutaGroupController::getCenter()
{
  if (this->stackMutas.empty())
    return BWAPI::Positions::None;
  BWAPI::Position center;
  for (Unit* unit: this->stackMutas)
    center += unit->getPosition();
  center.x /= this->stackMutas.size();
  center.y /= this->stackMutas.size();
  return center;
}

void MutaGroupController::sendJoiningMutas()
{
  BWAPI::Position center = this->getCenter();
  for (Unit* unit: this->joiningMutas)
    unit->move(center);
}

void MutaGroupController::transferJoiningMutasToStackMutas()
{
  BWAPI::Position center = this->getCenter();
  for (uint32_t i = 0; i < this->joiningMutas.size();)
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
