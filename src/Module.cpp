#include <Module.hpp>
#include <ModuleContainer.hpp>

Module::Module(ModuleContainer& owner)
  : owner(&owner)
{
  this->owner->add(this);
}

Module::Module()
{}

void Module::registerTo(ModuleContainer& container)
{
  if (this->owner)
    this->owner->remove(this);
  this->owner = &container;
  this->owner->add(this);
}

void Module::unregisterFrom()
{
  if (this->owner)
    this->owner->remove(this);
  this->owner = nullptr;
}

Module::~Module()
{
  if (this->owner)
    this->owner->remove(this);
}
