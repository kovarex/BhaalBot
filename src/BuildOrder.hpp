#pragma once
#include "BuildOrderItem.hpp"
class BuildOrderItem;

class BuildOrder
{
public:
  BuildOrder(const std::string& name) : name(name) {}
  BuildOrder() {}
  ~BuildOrder();
  void add(BWAPI::UnitType unit, BuildLocationType location = BuildLocationType::Auto);
  void add(uint32_t count, BWAPI::UnitType unit);
  void add(BuildOrderItem* item);

  std::string name;
  std::vector<BuildOrderItem*> items;
};
