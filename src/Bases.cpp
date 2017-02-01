#include <Bases.hpp>
#include <Base.hpp>
#include <BWEM/bwem.h>
#include <Log.hpp>

Bases::Bases(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

Bases::~Bases()
{
  for (Base* base: this->bases)
    delete base;
}

void Bases::init()
{
  BWAPI::Position myStartingLocationPosition = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
  for (const BWEM::Area& area: BWEM::Map::Instance().Areas())
    for (const BWEM::Base& bwemBase: area.Bases())
    {
      Base* base = new Base(&bwemBase);
      this->bases.insert(base);
      if (base->getCenter().getDistance(myStartingLocationPosition) < 100)
      {
        if (this->startingBase != nullptr)
          LOG_AND_ABORT("Two starting bases.");
        this->startingBase = base;
        this->startingBase->status = Base::Status::OwnedByMe;
      }
    }
  this->initAccessibilityFromStartingBase();
  this->initStartingLocations();
}

void Bases::initAccessibilityFromStartingBase()
{
  for (Base* base: this->bases)
  {
    if (base == this->startingBase)
      continue;
    int length = 0;
    auto& path = BWEM::Map::Instance().GetPath(this->startingBase->getCenter(), base->getCenter(), &length);
    if (length == -1)
      base->accessibleOnGround = false;
  }
}

void Bases::initStartingLocations()
{
  for (BWAPI::TilePosition startingLocation: BWEM::Map::Instance().StartingLocations())
  {
    Base* base = this->getClosestBase(BWAPI::Position(startingLocation));
    if (base == this->startingBase)
      continue;
    this->startingLocations.insert(base);
  }
}

Base* Bases::getClosestBase(BWAPI::Position position)
{
  Base* bestCandidate = nullptr;
  double bestCandidateDistance = 0;
  for (Base* base: this->bases)
  {
    double distance = base->getCenter().getDistance(position);
    if (bestCandidate == nullptr ||
        distance < bestCandidateDistance)
    {
      bestCandidateDistance = distance;
      bestCandidate = base;
    }
  }
  return bestCandidate;
}

Base* Bases::getEnemyBaseClosestTo(BWAPI::Position position)
{
  Base* candidate = nullptr;
  for (Base* base: this->bases)
    if (base->status == Base::Status::OwnedByEnemy &&
        (candidate == nullptr || candidate->getCenter().getDistance(position) > base->getCenter().getDistance(position)))
      candidate = base;
  return candidate;
}

void Bases::onFrame()
{
  for (Base* base: this->bases)
  {
    BWAPI::Broodwar->drawCircleMap(base->getCenter(), 60, BWAPI::Colors::Orange);
    std::string text;
    switch (base->status)
    {
     case Base::Status::Unknown: text = "Unknown base"; break;
     case Base::Status::OwnedByMe: text = "My base"; break;
     case Base::Status::Empty: text = "Empty base"; break;
     case Base::Status::OwnedByEnemy: text = "Enemy base"; break;
    }
    BWAPI::Broodwar->drawTextMap(base->getCenter(), "%s", text.c_str());
    base->onFrame();
  }
}

Base* Bases::enemyMainBase()
{
  for (Base* base: this->startingLocations)
    if (base->status == Base::Status::OwnedByEnemy)
      return base;
  return nullptr;
}
