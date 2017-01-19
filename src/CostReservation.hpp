#pragma once
#include <Cost.hpp>
#include <string>
#include <vector>
#include <Module.hpp>
class CostReservation;

class CostReservationItem
{
public:
  CostReservationItem(Cost cost);
  virtual ~CostReservationItem();
  virtual std::string str() const = 0;

  Cost cost;
  CostReservation* owner;
};

class CostReservation : Module
{
public:
  CostReservation(ModuleContainer& moduleContainer);
  ~CostReservation();
  void registerItem(CostReservationItem* item);
  void unregisterItem(CostReservationItem* item);
  Cost getReseved() const { return this->reserved; }
  void onFrame() override;
  bool canAfford(const Cost& cost) const;
private:
  void erase(CostReservationItem* item);

  std::vector<CostReservationItem*> items;
  Cost reserved;
};
