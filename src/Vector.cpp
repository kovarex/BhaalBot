#include <Vector.hpp>

Vector::Vector(BWAPI::Position from, BWAPI::Position to)
  : dx(to.x - from.x)
  , dy(to.y - from.y)
{}

void Vector::extendToLength(double length)
{
  double lengthSquared = length * length;
  double thisLengthSquared = this->dx * this->dx + this->dy * this->dy;
  if (thisLengthSquared == 0)
  {
    this->dy = length;
    return;
  }
  double ratio = sqrt(lengthSquared) / sqrt(thisLengthSquared);
  this->dx *= ratio;
  this->dy *= ratio;
}
