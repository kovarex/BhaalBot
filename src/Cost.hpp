#pragma once
#include <stdint.h>
#include <string>

class Cost
{
public:
  Cost() = default;
  Cost(uint32_t minerals, uint32_t gas) : minerals(minerals), gas(gas) {}
  void operator+=(Cost cost) { this->minerals += cost.minerals; this->gas += cost.gas; }
  Cost operator+(Cost cost) { Cost result(*this); result += cost; return result; }
  void operator-=(Cost cost) { this->minerals -= cost.minerals; this->gas -= cost.gas; }
  bool operator>=(const Cost& cost) { return this->minerals >= cost.minerals && this->gas >= cost.gas; }
  bool operator<(const Cost& cost) { return this->minerals < cost.minerals || this->gas < cost.gas; }
  std::string str() const;

  uint32_t minerals = 0;
  uint32_t gas = 0;
};
