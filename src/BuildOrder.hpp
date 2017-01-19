#pragma once
#include "BuildOrderItem.hpp"
class BuildOrderItem;

class BuildOrder
{
public:
  ~BuildOrder();
  void add(BWAPI::UnitType unit, BuildLocationType location = BuildLocationType::Auto);
  void add(uint32_t count, BWAPI::UnitType unit);
  void add(BuildOrderItem* item);

  std::vector<BuildOrderItem*> items;
};
