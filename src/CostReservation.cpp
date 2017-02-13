#include <CostReservation.hpp>
#include <BhaalBot.hpp>
#include <Log.hpp>

CostReservation::CostReservation(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

CostReservation::~CostReservation()
{
  for (CostReservationItem* item: this->items)
    item->owner = nullptr;
}

void CostReservation::registerItem(CostReservationItem* item)
{
  this->items.push_back(item);
  this->reserved += item->cost;
}

void CostReservation::unregisterItem(CostReservationItem* item)
{
  this->erase(item);
  this->reserved -= item->cost;
}

void CostReservation::onFrame()
{
  BWAPI::Broodwar->registerEvent([this](BWAPI::Game*)
                            {
                              BWAPI::Broodwar->drawText(BWAPI::CoordinateType::Screen,
                                                          200, 0,
                                                          "Reserved %s", this->reserved.str().c_str());

                              for (uint32_t i = 0; i < this->items.size(); ++i)
                                BWAPI::Broodwar->drawText(BWAPI::CoordinateType::Screen,
                                                          210, (i + 1) * 20,
                                                          "%d) %s for %s",
                                                          i + 1,
                                                          this->items[i]->str().c_str(),
                                                          this->items[i]->cost.str().c_str());
                            },
                            nullptr,  // condition
                            BWAPI::Broodwar->getLatencyFrames());
}

bool CostReservation::canAfford(const Cost& cost) const
{
  Cost required(cost);
  required += this->reserved;
  return Cost(BWAPI::Broodwar->self()->minerals(), BWAPI::Broodwar->self()->gas()) >= required;
}

void CostReservation::erase(CostReservationItem* item)
{
  for (uint32_t i = 0; i < this->items.size(); ++i)
    if (this->items[i] == item)
    {
      this->items.erase(this->items.begin() + i);
      return;
    }
  LOG_AND_ABORT("Trying to remove cost reservation item that is not present.");
}

CostReservationItem::CostReservationItem(Cost cost)
  : cost(cost)
  , owner(&bhaalBot->costReservation)
{
  this->owner->registerItem(this);
}

CostReservationItem::~CostReservationItem()
{
  this->clear();
}

void CostReservationItem::clear()
{
  if (!this->owner)
    return;
  this->owner->unregisterItem(this);
  this->owner = nullptr;
}
