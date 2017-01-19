#include <BaseHarvestingManager.hpp>
#include <Unit.hpp>

BaseHarvestingManager::BaseHarvestingManager(Unit* base, const BWEM::Base* bwemBase)
  : base(base)
  , bwemBase(bwemBase)
{
  for (BWEM::Mineral* mineral: this->bwemBase->Minerals())
    this->minerals.push_back(Mineral(mineral->Unit()));
  for (BWEM::Geyser* geyser: this->bwemBase->Geysers())
    this->geysers.push_back(Geyser(geyser->Unit()));
}

void BaseHarvestingManager::assignMiner(Unit* unit)
{
  Mineral* bestMineral = this->getBestMineral();
  if (bestMineral != nullptr)
  {
    bestMineral->miners.push_back(unit);
    unit->assign(new MineralHarvestingAssignment(this));
  }
}

void BaseHarvestingManager::update()
{
  //this->drawDebug();
  for (Mineral& mineral: this->minerals)
    for (Unit* miner: mineral.miners)
    {
      if (miner->isCarryingMinerals())
      {
        if (!this->base->equals(miner->getOrderTarget()))
          miner->rightClick(this->base->getBWAPIUnit());
      }
      else if (miner->getOrderTarget() != mineral.mineral)
          miner->gather(mineral.mineral);
    }
  for (Geyser& geyser: this->geysers)
    for (Unit* miner: geyser.miners)
    {
      if (miner->isIdle() ||
          !miner->isGatheringGas() ||
          (miner->getTarget() != geyser.geyser && !this->base->equals(miner->getTarget())))
        miner->gather(geyser.geyser);
    }
}

void BaseHarvestingManager::drawDebug()
{
  uint32_t index = 0;
  for (Mineral& mineral: this->minerals)
  {
    BWAPI::Broodwar->drawText(BWAPI::CoordinateType::Map,
                              mineral.mineral->getPosition().x,
                              mineral.mineral->getPosition().y,
                              "%d", index++);
  }
  BWAPI::Position against = this->againstMinerals();
  BWAPI::Broodwar->drawCircle(BWAPI::CoordinateType::Map,
                              against.x, against.y, 10, BWAPI::Color(255, 0, 0));
}

void BaseHarvestingManager::onUnitComplete(Unit* unit)
{
  if (unit->getType() == BWAPI::UnitTypes::Zerg_Extractor)
    for (Geyser& geyser: this->geysers)
      if (geyser.geyser->getPosition() == unit->getPosition())
      {
        geyser.state = Geyser::State::Ready;
        return;
      }
}

void BaseHarvestingManager::unassignFromMinerals(Unit* unit)
{
  for (Mineral& mineral: this->minerals)
    for (auto it = mineral.miners.begin(); it != mineral.miners.end();)
    {
      if (*it == unit)
      {
        mineral.miners.erase(it);
        return;
      }
      else
        ++it;
    }
  throw std::runtime_error("Trying to unassign mineral worker that is not here.");
}

void BaseHarvestingManager::unassignFromGas(Unit* unit)
{
  for (Geyser& geyser: this->geysers)
    for (auto it = geyser.miners.begin(); it != geyser.miners.end();)
    {
      if (*it == unit)
      {
        geyser.miners.erase(it);
        return;
      }
      else
        ++it;
    }
  throw std::runtime_error("Trying to unassign gas worker that is not here.");
}

BaseHarvestingManager::Mineral* BaseHarvestingManager::getBestMineral()
{
  if (this->minerals.empty())
    return nullptr;
  Mineral* bestMineral = nullptr;
  for (Mineral& mineral: this->minerals)
  {
    if (bestMineral == nullptr)
    {
      bestMineral = &mineral;
      continue;
    }
    if (bestMineral->miners.size() != mineral.miners.size())
    {
      if (bestMineral->miners.size() > mineral.miners.size())
        bestMineral = &mineral;
      continue;
    }
    if (this->base->getDistance(bestMineral->mineral) > this->base->getDistance(mineral.mineral))
    {
      bestMineral = &mineral;
      continue;
    }
  }
  return bestMineral;
}

uint32_t BaseHarvestingManager::smallestMineralSaturation() const
{
  uint32_t result = uint32_t(-1);
  for (const Mineral& mineral: this->minerals)
    if (mineral.miners.size() < result)
      result = mineral.miners.size();
  return result;
}

uint32_t BaseHarvestingManager::biggestMineralSaturation() const
{
  uint32_t result = 0;
  for (const Mineral& mineral: this->minerals)
    if (mineral.miners.size() > result)
      result = mineral.miners.size();
  return result;
}

Unit* BaseHarvestingManager::freeLeastNeededWorker()
{
  Mineral* mostSatisfiedMineral = nullptr;
  for (Mineral& mineral: this->minerals)
    if (mostSatisfiedMineral == nullptr ||
        mineral.miners.size() > mostSatisfiedMineral->miners.size())
      mostSatisfiedMineral = &mineral;
  if (mostSatisfiedMineral == nullptr)
    return nullptr;
  Unit* bestWorkerToFree = nullptr;
  uint32_t bestWorkerToFreeIndex = 0;
  for (uint32_t i = 0; i < mostSatisfiedMineral->miners.size(); ++i)
  {
    Unit* miner = mostSatisfiedMineral->miners[i];
    if (bestWorkerToFree == nullptr ||
        bestWorkerToFree->isGatheringMinerals() && !miner->isGatheringMinerals() ||
        bestWorkerToFree->isGatheringMinerals() && !miner->isGatheringMinerals())
    {
      bestWorkerToFree = miner;
      bestWorkerToFreeIndex = i;
    }
  }
  
  if (bestWorkerToFree != nullptr)
    bestWorkerToFree->assign(nullptr);
  return bestWorkerToFree;
}

BWAPI::Position BaseHarvestingManager::againstMinerals()
{
  BWAPI::Position basePosition = this->base->getPosition();
  std::vector<BWAPI::Position> oppositePositions;
  int xSum = 0, ySum = 0;
  int count = 0;
  for (const Mineral& mineral: this->minerals)
  {
    BWAPI::Position mineralPosition = mineral.mineral->getPosition();
    if (mineralPosition == BWAPI::Positions::Unknown)
      continue;
    BWAPI::Position vector = basePosition - mineralPosition;
    vector.x /= 2;
    vector.y /= 2;
    BWAPI::Position result = basePosition + vector;
    xSum += result.x;
    ySum += result.y;
    ++count;
  }
  if (count == 0)
    return BWAPI::Positions::None;
  return BWAPI::Position(xSum / count, ySum / count);
}

MineralHarvestingAssignment::~MineralHarvestingAssignment()
{
  this->base->unassignFromMinerals(this->unit);
}

GasHarvestingAssignment::~GasHarvestingAssignment()
{
  this->base->unassignFromGas(this->unit);
}
