#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
class Deserialiser;
class Direction;
class Serialiser;
class Vector;

class RealOrientation
{
public:
  RealOrientation();
  explicit RealOrientation(float orientation);
  RealOrientation(const Vector& vector);
  RealOrientation(const Direction& direction);
  inline float getFraction() const { return this->orientation; }
  float getAngle() const;
  RealOrientation operator+(const RealOrientation& realOrientation) const;
  RealOrientation operator+(float addition) const;
  bool operator==(const RealOrientation& other) const { return this->orientation == other.getFraction(); }
  bool operator==(float other) const { return this->orientation == other; }
  bool operator!=(const RealOrientation& other) const
  { return this->orientation != other.getFraction(); }
  void operator+=(const RealOrientation& orientation);
  void operator+=(float addition);
  void operator-=(const RealOrientation& orientation);
  void operator-=(float addition);
  RealOrientation operator-(const RealOrientation& orientation) const;
  RealOrientation operator-(float addition) const;
  Direction direction4Way() const;
  void moveTo(const RealOrientation& target, float speed);
  float moveToAndReturnMovement(const RealOrientation& target, float speed);
  inline float distanceTo(const RealOrientation& second) const
  {
    float x = fabs(this->orientation - second.orientation);
    return std::min(x, float(fabs(x - 1)));
  }
  float signedDistanceTo(const RealOrientation& second) const;
private:
  float orientation;
  inline void normalise()
  {
    if (this->orientation >= 0 && this->orientation < 1)
      return;
    this->orientation = (this->orientation - (int32_t) this->orientation) + 1;
    this->orientation = (this->orientation - (int32_t) this->orientation);
    assert(this->orientation >= 0 && this->orientation < 1);
  }
};
