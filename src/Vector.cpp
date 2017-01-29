#include <Vector.hpp>

std::vector<Vector> Vector::basicOneTileDirections = {Vector(0, -32), Vector(32, 0), Vector(0, 32), Vector(-32, 0) };

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
  double thisLength = this->getLength();
  this->dx *= length / thisLength;
  this->dy *= length / thisLength;
}
