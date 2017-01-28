#include <BaseInDangerDetector.hpp>
#include <Base.hpp>
#include <BhaalBot.hpp>
#include <DefendTaskForceController.hpp>
#include <HarvestingManager.hpp>
#include <BWEM/bwem.h>
#include <Unit.hpp>

HarvestingManager::HarvestingManager(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

HarvestingManager::~HarvestingManager()
{
  for (BaseHarvestingController* base: this->bases)
    delete base;
}

void HarvestingManager::onFrame()
{
  for (BaseHarvestingController* base: this->bases)
    base->update();
  this->balanceGasMining();
  this->balanceWorkersAssignmentBetweenBases();
}

void HarvestingManager::add(Unit* unit)
{
  if (this->bases.empty())
    this->baseLessMiners.push_back(unit);
  else
    this->bases.front()->assignMiner(unit);
}

void HarvestingManager::addBase(Unit* baseUnit, Base* base)
{
  this->bases.push_back(new BaseHarvestingController(baseUnit, base));
  for (Unit* unit: this->baseLessMiners)
    this->bases.back()->assignMiner(unit);
  this->baseLessMiners.clear();
}

Unit* HarvestingManager::getClosestWorker(BWAPI::Position position)
{
  Unit* closestUnit = nullptr;

  for (BaseHarvestingController* base: this->bases)
    for (BaseHarvestingController::Mineral* mineral: base->minerals)
      for (uint32_t i = 0; i < mineral->miners.size(); ++i)
      {
        Unit* worker = mineral->miners[i];
        if (closestUnit == nullptr ||
            closestUnit->getDistance(position) > worker->getDistance(position))
          closestUnit = worker;
      }
  return closestUnit;
}

bool HarvestingManager::hasBaseNearby(BWAPI::Position position) const
{
  for (const BaseHarvestingController* base: this->bases)
    if (base->baseUnit->getDistance(position) < 5)
      return true;
  return false;
}

float HarvestingManager::averageDistanceToBases(BWAPI::Position position) const
{
  float result = 0;
  for (const BaseHarvestingController* base: this->bases)
  {
    int length = 0;
    auto& path = BWEM::Map::Instance().GetPath(position, base->baseUnit->getPosition(), &length);
    result += length;
  }
  return result / this->bases.size();
}

BaseHarvestingController::Geyser* HarvestingManager::getFreeGeyser()
{
  for (BaseHarvestingController* base: this->bases)
    for (BaseHarvestingController::Geyser* geyser: base->geysers)
      if (geyser->state == BaseHarvestingController::Geyser::State::Free)
        return geyser;
  return nullptr;
}

void HarvestingManager::balanceGasMining()
{
  uint32_t gasMiners = 0;
  for (BaseHarvestingController* base: this->bases)
    for (BaseHarvestingController::Geyser* geyser: base->geysers)
      gasMiners += geyser->miners.size();
  if (gasMiners == this->geyserMinersWanted)
    return;
  if (gasMiners < this->geyserMinersWanted)
  {
    BaseHarvestingController::Geyser* bestGeyser = nullptr;
    BaseHarvestingController* theBase = nullptr;
    for (BaseHarvestingController* base: this->bases)
      for (BaseHarvestingController::Geyser* geyser: base->geysers)
      {
        if (geyser->state != BaseHarvestingController::Geyser::State::Ready)
          continue;
        if (geyser->miners.size() >= 3)
          continue;
        if (bestGeyser == nullptr)
        {
          bestGeyser = geyser;
          theBase = base;
          continue;
        }
        if (bestGeyser->miners.size() > geyser->miners.size())
        {
          bestGeyser = geyser;
          theBase = base;
        }
      }
    if (bestGeyser == nullptr)
      return;
    Unit* worker = this->getClosestWorker(bestGeyser->geyser->getPosition());
    if (worker == nullptr)
      return;
    worker->assign(nullptr);
    bestGeyser->miners.push_back(worker);
    worker->assign(new GasHarvestingAssignment(theBase));
  }
}

void HarvestingManager::balanceWorkersAssignmentBetweenBases()
{
  if (this->bases.size() < 2)
    return;
  uint32_t smallestBaseSaturation = uint32_t(-1);
  BaseHarvestingController* smallestBase = nullptr;
  uint32_t biggestBaseSaturation = 0;
  BaseHarvestingController* biggestBase = nullptr;
  for (BaseHarvestingController* base: this->bases)
  {
    uint32_t smallestMineralSaturation = base->smallestMineralSaturation();
    if (smallestMineralSaturation < smallestBaseSaturation)
    {
      smallestBase = base;
      smallestBaseSaturation = smallestMineralSaturation;
    }
    uint32_t biggestMineralSaturation = base->biggestMineralSaturation();
    if (biggestMineralSaturation > biggestBaseSaturation)
    {
      biggestBase = base;
      biggestBaseSaturation = biggestMineralSaturation;
    }
  }
  if (biggestBaseSaturation > smallestBaseSaturation)
  {
    Unit* worker = biggestBase->getLeastNeededWorker();
    if (worker != nullptr)
      smallestBase->assignMiner(worker);
  }
}

void HarvestingManager::onUnitComplete(Unit* unit)
{
  for (BaseHarvestingController* base: this->bases)
    base->onUnitComplete(unit);
  if (unit->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
  {
    Base* base = bhaalBot->bases.getClosestBase(unit->getPosition());
    if (base == nullptr)
      return;
    if (base->getCenter().getDistance(unit->getPosition()) > 100)
      return; // It is too far from mining location
    for (BaseHarvestingController* myBase: this->bases)
      if (myBase->base == base)
        return; // I already have this base registered
    base->status = Base::Status::OwnedByMe;
    base->baseInDangerDetector = new BaseInDangerDetector(*base);
    base->defendForce = new TaskForce();
    bhaalBot->fightManager.taskForces.push_back(base->defendForce);
    base->defendForce->assignTaskController(new DefendTaskForceController(*base->defendForce, base));
    std::vector<BWAPI::Unit> minerals;
    for (BWEM::Mineral* mineral: base->getBWEMBase()->Minerals())
      minerals.push_back(mineral->Unit());

    std::vector<BWAPI::Unit> geysers;
    for (BWEM::Geyser* geyser: base->getBWEMBase()->Geysers())
      geysers.push_back(geyser->Unit());
    this->addBase(unit, base);
    return;
  }
}

void HarvestingManager::onUnitDestroy(Unit* unit)
{
  for (BaseHarvestingController* base: this->bases)
    base->onUnitDestroy(unit);
  if (unit->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
    for (auto it = this->bases.begin(); it != this->bases.end(); ++it)
      if ((*it)->baseUnit == unit)
      {
        Base* base = (*it)->base;
        delete base->baseInDangerDetector;
        base->baseInDangerDetector = nullptr;
        if (base->defendForce)
        {
          for (auto it = bhaalBot->fightManager.taskForces.begin(); it != bhaalBot->fightManager.taskForces.end(); ++it)
            if (*it == base->defendForce)
            {
              bhaalBot->fightManager.taskForces.erase(it);
              break;
            }
          delete base->defendForce;
          base->defendForce = nullptr;
        }
        delete *it;
        this->bases.erase(it);
        break;
      }
}
