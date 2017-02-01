#pragma once
#include <stdint.h>
#include <string>
#include <array>
class RealOrientation;

class Direction
{
public:
  enum class Enum : uint8_t
  {
    North = 0,
    NorthEast = 1,
    East = 2,
    SouthEast = 3,
    South = 4,
    SouthWest = 5,
    West = 6,
    NorthWest = 7,
    None = 8
  };

  // Trick to move the enum definitions to scope of Direction,
  // so the usage isn't Direction::Enum::North, but Direction::North.
  static constexpr Enum North = Enum::North;
  static constexpr Enum NorthEast = Enum::NorthEast;
  static constexpr Enum East = Enum::East;
  static constexpr Enum SouthEast = Enum::SouthEast;
  static constexpr Enum South = Enum::South;
  static constexpr Enum SouthWest = Enum::SouthWest;
  static constexpr Enum West = Enum::West;
  static constexpr Enum NorthWest = Enum::NorthWest;
  static constexpr Enum None = Enum::None;
  Direction() : value(Direction::None) {}
  Direction(const RealOrientation& orientation);
  Direction(Enum value) : value(value) {}
  bool operator==(const Enum value) const { return this->value == value; }
  bool operator==(const Direction& direction) const { return this->value == direction.value; }
  bool operator!=(const Enum value) const { return this->value != value; }
  bool operator!=(const Direction& direction) const { return this->value != direction.value; }
  void operator--();
  void operator++();
  void operator+=(uint8_t stepCount);
  void operator-=(uint8_t stepCount);
  Direction operator-(const Direction& direction) const;
  Direction operator+(const Direction& direction) const;
  operator Enum() const { return this->value; }
  uint8_t getIndex() const { return uint8_t(this->value); }
  INLINE_CPP uint8_t getStraightIndex() const;
  RealOrientation orientation() const;
  Direction opposite() const { return this->rotate(4); }
  bool isDiagonal() const;
  bool isHorizontal() const { return *this == East || *this == West; }
  bool isVertical() const { return uint8_t(this->value) % 4 == 0; }
  Direction verticalHorizontalMinimisation() const;
  Direction right90() const { return this->rotate(2); }
  Direction right45() const { return this->rotate(1); }
  Direction left90() const { return this->rotate(-2); }
  Direction left45() const { return this->rotate(-1); }
  void getOffset(int32_t& x, int32_t& y) const;
  Direction rotate(int32_t rotation) const { return Direction(Enum((uint8_t(this->value) + rotation) & 7)); }
  Direction rotate(Direction rotation) const { return Direction(Enum((uint8_t(this->value) + rotation.getIndex()) & 7)); }
  Direction withoutDiagonal() const { return Direction(Enum(this->getIndex() & ~uint8_t(1))); }

  bool isZero() const { return this->value == Direction::None; }

  std::string str() const;

  static const std::array<Direction, 4> straightDirections;
  static const std::array<Direction, 8> allDirections;

private:
  Enum value;
};
