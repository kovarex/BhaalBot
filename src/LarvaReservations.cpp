#include <LarvaReservations.hpp>
#include <StringUtil.hpp>
#include <Unit.hpp>
#include <BhaalBot.hpp>
#include <Log.hpp>

LarvaReservations::MorphTaskInProgress::MorphTaskInProgress(Unit* larva, BWAPI::UnitType targetUnit)
  : CostReservationItem(Cost(targetUnit.mineralPrice(),
                             targetUnit.gasPrice()))
  , larvaToBeUsed(larva)
  , targetUnit(targetUnit)
  , parentHatchery(bhaalBot->units.find(larva->getHatchery()))
  , tickOfLastOrder(BWAPI::Broodwar->getFrameCount())
{}

std::string LarvaReservations::MorphTaskInProgress::str() const
{
  return ssprintf("Morphing %s", this->targetUnit.getName().c_str());
}

bool LarvaReservations::MorphTaskInProgress::morhphingStarted() const
{
  return this->larvaToBeUsed->getType() != BWAPI::UnitTypes::Zerg_Larva;
}

void LarvaReservations::MorphTaskInProgress::onFrame() const
{
  if (BWAPI::Broodwar->getFrameCount() - this->tickOfLastOrder > BWAPI::Broodwar->getLatencyFrames())
    this->larvaToBeUsed->train(this->targetUnit);
}

LarvaReservations::LarvaReservations(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

LarvaReservations::~LarvaReservations()
{
  for (MorphTaskInProgress* morphTask: this->morphTasks)
    delete morphTask;
}

bool LarvaReservations::tryToTrain(Unit* hatchery, BWAPI::UnitType targetUnit)
{
  for (BWAPI::Unit bwLarva: hatchery->getLarva())
  {
    Unit* larva = bhaalBot->units.find(bwLarva);
    if (!this->isResrved(larva) && larva->train(targetUnit))
    {
      this->registerTask(larva, targetUnit);
      return true;
    }
  }
  return false;
}

void LarvaReservations::registerTask(Unit* larva, BWAPI::UnitType targetUnit)
{
  LOG_INFO("Training %s from larva id %u", targetUnit.getName().c_str(), larva->getBWAPIUnit()->getID());
  this->morphTasks.push_back(new MorphTaskInProgress(larva, targetUnit));
  this->reservedByProducers[this->morphTasks.back()->parentHatchery]++;
  bhaalBot->morphingUnits.addPlannedMorph(targetUnit);
  this->larvasRegistered.insert(larva);
}

void LarvaReservations::onUnitDestroy(Unit* unit)
{
  if (unit->getType() == BWAPI::UnitTypes::Zerg_Larva)
    for (uint32_t i = 0; i < this->morphTasks.size();++i)
      if (this->morphTasks[i]->larvaToBeUsed == unit)
      {
        delete this->morphTasks[i];
        this->morphTasks.erase(this->morphTasks.begin() + i);
        return;
      }
}

void LarvaReservations::onFrame()
{
  for (uint32_t i = 0; i < this->morphTasks.size();)
  {
    if (this->morphTasks[i]->morhphingStarted())
    {
      MorphTaskInProgress* morphTask = this->morphTasks[i];
      this->reservedByProducers[morphTask->parentHatchery]--;
      this->larvasRegistered.erase(morphTask->larvaToBeUsed);
      bhaalBot->morphingUnits.removePlannedMorph(morphTask->targetUnit);
      delete this->morphTasks[i];
      this->morphTasks.erase(this->morphTasks.begin() + i);
    }
    else
    {
      this->morphTasks[i]->onFrame();
      ++i;
    }
  }
}

int32_t LarvaReservations::reservedLarvas(Unit* hatch)
{
  auto position = this->reservedByProducers.find(hatch);
  if (position == this->reservedByProducers.end())
    return 0;
  return position->second;
}

bool LarvaReservations::isResrved(Unit* larva)
{
  return this->larvasRegistered.count(larva) != 0;
}
