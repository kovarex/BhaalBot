#include <Bases.hpp>
#include <Base.hpp>
#include <BWEM/bwem.h>
#include <Log.hpp>

Bases::Bases(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

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
    base->startingBaseStatus = Base::StartingBaseStatus::Unknown;
  }
}

Base* Bases::getClosestBase(BWAPI::Position position)
{
  for (Base* base: this->bases)
    if (base->getCenter().getDistance(position) < 150)
      return base;
  return nullptr;
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
    if (base->startingBaseStatus == Base::StartingBaseStatus::Enemy)
      return base;
  return nullptr;
}
