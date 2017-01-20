#pragma once
#include <BWAPI.h>

class Vector
{
public:
  Vector(double dx, double dy);
  Vector(BWAPI::Position from, BWAPI::Position to);
  void extendToLength(double length);
  void addTo(BWAPI::Position* position) { position->x += int(this->dx); position->y += int(this->dy); }
  double getLength() const { return sqrt(this->dx * this->dx + this->dy * this->dy); }

  double dx = 0, dy = 0;
};
