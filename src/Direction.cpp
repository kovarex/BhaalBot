#include <Log.hpp>
#include <Direction.hpp>
#include <RealOrientation.hpp>
#include <StringUtil.hpp>
#include <assert.h>

const std::array<Direction, 4> Direction::straightDirections =
{
  Direction::North,
  Direction::East,
  Direction::South,
  Direction::West
};

const std::array<Direction, 8> Direction::allDirections =
{
  Direction::North,
  Direction::NorthEast,
  Direction::East,
  Direction::SouthEast,
  Direction::South,
  Direction::SouthWest,
  Direction::West,
  Direction::NorthWest
};

void Direction::operator--()
{
  this->value = Enum((uint8_t(this->value) - 1) & 7);
}

void Direction::operator++()
{
  this->value = Enum((uint8_t(this->value) + 1) & 7);
}

void Direction::operator+=(uint8_t stepCount)
{
  this->value = Enum((uint8_t(this->value) + stepCount) & 7);
}

void Direction::operator-=(uint8_t stepCount)
{
  this->value = Enum((uint8_t(this->value) - stepCount) & 7);
}

Direction::Direction(const RealOrientation& realOrientation)
  : value(Enum(uint8_t(8 + realOrientation.getFraction() * 8 + 0.5) & 7))
{}

RealOrientation Direction::orientation() const
{
  return RealOrientation(this->getIndex() * (1 / float(8.0)));
}

bool Direction::isDiagonal() const
{
  return *this == NorthEast ||
         *this == SouthEast ||
         *this == NorthWest ||
         *this == SouthWest;
}

Direction Direction::operator-(const Direction& direction) const
{
  return Direction(Enum((this->getIndex() - direction.getIndex()) & 7));
}

Direction Direction::operator+(const Direction& direction) const
{
  return Direction(Enum((this->getIndex() + direction.getIndex()) & 7));
}

void Direction::getOffset(int32_t& x, int32_t& y) const
{
  switch (this->value)
  {
    case North:
      x = 0; y = -1; break;
    case NorthEast:
      x = 1; y = -1; break;
    case East:
      x = 1; y = 0; break;
    case SouthEast:
      x = 1; y = 1; break;
    case South:
      x = 0; y = 1; break;
    case SouthWest:
      x = -1; y = 1; break;
    case West:
      x = -1; y = 0; break;
    case NorthWest:
      x = -1; y = -1; break;
    default:
      x = 0; y = 0; break;
  }
}

std::string Direction::str() const
{
  switch (this->value)
  {
    case North:
      return "North";
    case NorthEast:
      return "NorthEast";
    case East:
      return "East";
    case SouthEast:
      return "SouthEast";
    case South:
      return "South";
    case SouthWest:
      return "SouthWest";
    case West:
      return "West";
    case NorthWest:
      return "NorthWest";
    default:
      return "";
  }
}

Direction Direction::verticalHorizontalMinimisation() const
{
  switch (this->value)
  {
    case North:
    case NorthEast:
    case South:
    case SouthWest:
      return Direction::North;
    case East:
    case SouthEast:
    case West:
    case NorthWest:
      return Direction::East;
    default:
      LOG_AND_ABORT("Invalid direction in verticalHorizontalMinimisation");
  }
}

INLINE_CPP uint8_t Direction::getStraightIndex() const
{
  assert(!this->isDiagonal()); return this->getIndex() / 2;
}
