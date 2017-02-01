#include <Direction.hpp>
#include <RealOrientation.hpp>
#include <StringUtil.hpp>
#include <Vector.hpp>

RealOrientation::RealOrientation()
  : orientation(0)
{}

RealOrientation::RealOrientation(float orientation)
  : orientation(orientation)
{
  this->normalise();
}

void RealOrientation::operator+=(float addition)
{
  this->orientation += addition;
  this->normalise();
}

void RealOrientation::operator+=(const RealOrientation& orientation)
{
  this->orientation += orientation.orientation;
  this->normalise();
}

RealOrientation RealOrientation::operator+(const RealOrientation& realOrientation) const
{
  RealOrientation result(*this);
  result += realOrientation.getFraction();
  return result;
}

RealOrientation RealOrientation::operator+(float addition) const
{
  RealOrientation result(*this);
  result += addition;
  return result;
}

void RealOrientation::operator-=(float addition)
{
  this->orientation -= addition;
  this->normalise();
}

void RealOrientation::operator-=(const RealOrientation& orientation)
{
  this->orientation -= orientation.getFraction();
  this->normalise();
}

RealOrientation RealOrientation::operator-(const RealOrientation& orientation) const
{
  RealOrientation result(*this);
  result -= orientation;
  return result;
}

RealOrientation RealOrientation::operator-(float addition) const
{
  RealOrientation result(*this);
  result -= addition;
  return result;
}

RealOrientation::RealOrientation(const Vector& vector)
{
  // assert(!vector.isZero());
  this->orientation = float(atan2(vector.dx, -vector.dy) * M_1_PI / 2);
  this->normalise();
}

void RealOrientation::moveTo(const RealOrientation& target, float speed)
{
  if (speed >= 0.5)
  {
    this->orientation = target.orientation;
    return;
  }

  if (this->orientation > target.getFraction())
  {
    if (this->orientation - target.getFraction() < 0.5)
    {
      this->orientation = std::max(target.getFraction(), this->orientation - speed);
      this->normalise();
      assert(this->orientation >= 0 && this->orientation < 1);
    }
    else
    {
      (*this) += speed;
      if (speed > this->orientation && this->orientation > target.getFraction())
        this->orientation = target.getFraction();
    }
  }
  else
  {
    if (target.getFraction() - this->orientation < 0.5)
    {
      this->orientation = std::min(target.getFraction(), this->orientation + speed);
      this->normalise();
      assert(this->orientation >= 0 && this->orientation < 1);
    }
    else
    {
      (*this) -= speed;
      if (speed > 1 - this->orientation && this->orientation < target.getFraction())
        this->orientation = target.getFraction();
    }
  }
}

float RealOrientation::moveToAndReturnMovement(const RealOrientation& target, float speed)
{
  if (speed >= 0.5)
  {
    float result = fabs(target.getFraction() - this->getFraction());
    if (this->getFraction() > target.getFraction())
      result = std::min(result, 1 - this->getFraction() + target.getFraction());
    else
      result = std::min(result, 1 - target.getFraction() + this->getFraction());
    this->orientation = target.orientation;
    return result;
  }

  if (this->orientation > target.getFraction())
  {
    if (this->orientation - target.getFraction() < 0.5)
    {
      float movement = std::min(speed, this->orientation - target.getFraction());
      this->orientation -= movement;
      return movement;
    }
    else
    {
      float movement = std::min(speed, 1 - this->orientation + target.getFraction());
      (*this) += movement;
      return movement;
    }
  }
  else
  {
    if (target.getFraction() - this->orientation < 0.5)
    {
      float movement = std::min(speed, target.getFraction() - this->orientation);
      this->orientation += movement;
      return movement;
    }
    else
    {
      float movement = std::min(speed, 1 - target.getFraction() + this->orientation);
      (*this) -= movement;
      return movement;
    }
  }
}

RealOrientation::RealOrientation(const Direction& direction)
  : orientation(direction.getIndex() * (1 / float(8.0)))
{}

float RealOrientation::signedDistanceTo(const RealOrientation& second) const
{
  float distance = second.orientation - this->orientation;
  if (fabs(distance) <= 0.5)
    return distance;
  else if (distance >= 0)
    return distance - 1;
  else
    return distance + 1;
}

Direction RealOrientation::direction4Way() const
{
  static float intPart;
  return Direction (Direction::Enum(2 * int32_t(modff(this->orientation + 1.0f / 8, &intPart) * 4)));
}

float RealOrientation::getAngle() const
{
  return float(this->orientation * 2 * M_PI);
}
