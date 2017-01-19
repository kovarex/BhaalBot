#pragma once
#include <string>
class Unit;

class Assignment
{
public:
  virtual ~Assignment() {}
  virtual std::string str() const = 0;

  Unit* unit = nullptr;
};
