#include <HarvestingManager.hpp>
#include <BWEM/bwem.h>
#include <Unit.hpp>

HarvestingManager::HarvestingManager(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

HarvestingManager::~HarvestingManager()
{
  for (BaseHarvestingManager* base: this->bases)
    delete base;
}

void HarvestingManager::onFrame()
{
  for (BaseHarvestingManager* base: this->bases)
    base->update();
  this->balanceGasMining();
  this->balanceWorkersAssignmentBetweenBases();
}

void HarvestingManager::onAdded(Unit* unit)
{
  if (this->bases.empty())
    this->baseLessMiners.push_back(unit);
  else
    this->bases.front()->assignMiner(unit);
}

void HarvestingManager::addBase(Unit* base, const BWEM::Base* bwemBase)
{
  this->bases.push_back(new BaseHarvestingManager(base, bwemBase));
  for (Unit* unit: this->baseLessMiners)
    this->bases.back()->assignMiner(unit);
  this->baseLessMiners.clear();
}

Unit* HarvestingManager::getClosestWorker(BWAPI::Position position)
{
  Unit* closestUnit = nullptr;

  for (BaseHarvestingManager* base: this->bases)
    for (BaseHarvestingManager::Mineral& mineral: base->minerals)
      for (uint32_t i = 0; i < mineral.miners.size(); ++i)
      {
        Unit* worker = mineral.miners[i];
        if (closestUnit == nullptr ||
            closestUnit->getDistance(position) > worker->getDistance(position))
          closestUnit = worker;
      }
  return closestUnit;
}

bool HarvestingManager::hasBaseNearby(BWAPI::Position position) const
{
  for (const BaseHarvestingManager* base: this->bases)
    if (base->base->getDistance(position) < 5)
      return true;
  return false;
}

float HarvestingManager::averageDistanceToBases(BWAPI::Position position) const
{
  float result = 0;
  for (const BaseHarvestingManager* base: this->bases)
  {
    int length = 0;
    auto& path = BWEM::Map::Instance().GetPath(position, base->base->getPosition(), &length);
    result += length;
  }
  return result / this->bases.size();
}

BaseHarvestingManager::Geyser* HarvestingManager::getFreeGeyser()
{
  for (BaseHarvestingManager* base: this->bases)
    for (BaseHarvestingManager::Geyser& geyser: base->geysers)
      if (geyser.state == BaseHarvestingManager::Geyser::State::Free)
        return &geyser;
  return nullptr;
}

void HarvestingManager::balanceGasMining()
{
  uint32_t gasMiners = 0;
  for (BaseHarvestingManager* base: this->bases)
    for (BaseHarvestingManager::Geyser& geyser: base->geysers)
      gasMiners += geyser.miners.size();
  if (gasMiners == this->geyserMinersWanted)
    return;
  if (gasMiners < this->geyserMinersWanted)
  {
    BaseHarvestingManager::Geyser* bestGeyser = nullptr;
    BaseHarvestingManager* theBase = nullptr;
    for (BaseHarvestingManager* base: this->bases)
      for (BaseHarvestingManager::Geyser& geyser: base->geysers)
      {
        if (geyser.state != BaseHarvestingManager::Geyser::State::Ready)
          continue;
        if (geyser.miners.size() >= 3)
          continue;
        if (bestGeyser == nullptr)
        {
          bestGeyser = &geyser;
          theBase = base;
          continue;
        }
        if (bestGeyser->miners.size() > geyser.miners.size())
        {
          bestGeyser = &geyser;
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
  BaseHarvestingManager* smallestBase = nullptr;
  uint32_t biggestBaseSaturation = 0;
  BaseHarvestingManager* biggestBase = nullptr;
  for (BaseHarvestingManager* base: this->bases)
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
    Unit* worker = biggestBase->freeLeastNeededWorker();
    if (worker != nullptr)
      smallestBase->assignMiner(worker);
  }
}

void HarvestingManager::onUnitComplete(Unit* unit)
{
  if (unit->getPlayer() != BWAPI::Broodwar->self())
    return;
  for (BaseHarvestingManager* base: this->bases)
    base->onUnitComplete(unit);
  if (unit->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
  {
    const BWEM::Area* area = BWEM::Map::Instance().GetNearestArea(BWAPI::TilePosition(unit->getPosition()));
    for (const BWEM::Base& base: area->Bases())
    {
      if (base.Center().getDistance(unit->getPosition()) < 3)
      {
        for (BaseHarvestingManager* myBase: this->bases)
          if (myBase->bwemBase == &base)
            return; // I already have this base registered
        std::vector<BWAPI::Unit> minerals;
        for (BWEM::Mineral* mineral: base.Minerals())
          minerals.push_back(mineral->Unit());

        std::vector<BWAPI::Unit> geysers;
        for (BWEM::Geyser* geyser: base.Geysers())
          geysers.push_back(geyser->Unit());
        this->addBase(unit, &base);
        return;
      }
    }
  }
}
