#include <Module.hpp>
#include <ModuleContainer.hpp>

Module::Module(ModuleContainer& owner)
  : owner(owner)
{
  this->owner.add(this);
}

Module::~Module()
{
  this->owner.remove(this);
}
