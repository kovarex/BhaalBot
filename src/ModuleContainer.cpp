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

void ModuleContainer::onForeignUnitComplete(BWAPI::Unit unit)
{
  for (Module* module: this->modules)
    module->onForeignUnitComplete(unit);
}

void ModuleContainer::onUnitMorph(Unit* unit)
{
  for (Module* module: this->modules)
    module->onUnitMorph(unit);
}

void ModuleContainer::onFrame()
{
  for (Module* module: this->modules)
    module->onFrame();
}

void ModuleContainer::onStart()
{
  for (Module* module: this->modules)
    module->onStart();
}

void ModuleContainer::onForeignUnitDestroy(BWAPI::Unit unit)
{
  for (Module* module: this->modules)
    module->onForeignUnitDestroy(unit);
}

void ModuleContainer::onUnitIdle(Unit* unit)
{
  for (Module* module: this->modules)
    module->onUnitIdle(unit);
}

void ModuleContainer::onUnitDestroy(Unit* unit)
{
  for (Module* module: this->modules)
    module->onUnitDestroy(unit);
}
