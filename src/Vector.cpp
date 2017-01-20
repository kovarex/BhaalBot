#include <Vector.hpp>

Vector::Vector(double dx, double dy)
  : dx(dx)
  , dy(dy)
{}

Vector::Vector(BWAPI::Position from, BWAPI::Position to)
  : dx(to.x - from.x)
  , dy(to.y - from.y)
{}

void Vector::extendToLength(double length)
{
  if (this->dx == 0 && this->dy == 0)
  {
    this->dy = length;
    return;
  }
  this->dx *= length / this->getLength();
  this->dy *= length / this->getLength();
}
