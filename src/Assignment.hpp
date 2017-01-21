#pragma once
#include <string>
class Unit;

/** Specification of unit assignment.
 * It can be used to read what the assignment is and to unassign the unit from its task whatever it is. */
class Assignment
{
public:
  virtual ~Assignment() {} /**< Automatically unassigns the unit from its task. */
  virtual std::string str() const = 0; /**< @return String representation of the assignment */

  Unit* unit = nullptr; /**< The unit witht the assignement. This is set it Unit::assign. */
};
