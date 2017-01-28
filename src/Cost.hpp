#pragma once
#include <stdint.h>
#include <string>

class Cost
{
public:
  Cost() = default;
  Cost(int minerals, int gas) : minerals(minerals), gas(gas) {}
  void operator+=(Cost cost) { this->minerals += cost.minerals; this->gas += cost.gas; }
  Cost operator+(Cost cost) { Cost result(*this); result += cost; return result; }
  void operator-=(Cost cost) { this->minerals -= cost.minerals; this->gas -= cost.gas; }
  bool operator>=(const Cost& cost) { return this->minerals >= cost.minerals && this->gas >= cost.gas; }
  bool operator<(const Cost& cost) { return this->minerals < cost.minerals || this->gas < cost.gas; }
  std::string str() const;

  int minerals = 0;
  int gas = 0;
};
