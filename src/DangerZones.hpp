#pragma once
#include <Module.hpp>

class DangerZones : public Module
{
public:
  DangerZones(ModuleContainer& moduleContainer);
  void onStart() override;
  void onFrame() override;
  void addDanger(BWAPI::Position centerPosition, int32_t radius, int32_t value);
  void removeDanger(BWAPI::Position centerPosition, int32_t radius, int32_t value);

  std::vector<std::vector<int32_t>> data;
  int32_t width = 0, height = 0;
};
