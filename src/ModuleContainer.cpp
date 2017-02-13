#include <ModuleContainer.hpp>
#include <Module.hpp>

void ModuleContainer::add(Module* module)
{
  this->modules.push_back(module);
}

void ModuleContainer::remove(Module* module)
{
  for (auto it = this->modules.begin(); it != this->modules.end(); ++it)
    if (*it == module)
    {
      this->modules.erase(it);
      return;
    }
}

void ModuleContainer::onUnitComplete(Unit* unit)
{
  for (Module* module: this->modules)
    module->onUnitComplete(unit);
}

void ModuleContainer::onUnitCreate(Unit* unit)
{
  for (Module* module: this->modules)
    module->onUnitCreate(unit);
}

void ModuleContainer::onUnitDiscover(Unit* unit)
{
  for (Module* module: this->modules)
    module->onUnitDiscover(unit);
}

void ModuleContainer::onUnitMorph(Unit* unit, BWAPI::UnitType from)
{
  for (Module* module: this->modules)
    module->onUnitMorph(unit, from);
}

void ModuleContainer::onFrame()
{
  std::vector<Module*> copy(this->modules);
  for (Module* module: copy)
    module->onFrame();
}

void ModuleContainer::onStart()
{
  std::vector<Module*> copy(this->modules);
  for (Module* module: copy)
    module->onStart();
}

void ModuleContainer::onUnitIdle(Unit* unit)
{
  for (Module* module: this->modules)
    module->onUnitIdle(unit);
}

void ModuleContainer::onEnd(bool isWinner)
{
  std::vector<Module*> copy(this->modules);
  for (Module* module: copy)
    module->onEnd(isWinner);
}

void ModuleContainer::onUnitDestroy(Unit* unit)
{
  for (Module* module: this->modules)
    module->onUnitDestroy(unit);
}
