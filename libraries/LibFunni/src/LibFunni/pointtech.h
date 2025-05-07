#pragma once

#include <array>
#include <stdint.h>

// TODO: super unfinished
namespace funni {

enum class coord { x = 0, y = 1, z = 2 };

// base class for an nth dimensional coordinate expressed with n coordinates of
// (intended to be) numerical type T
template <typename T, int n> class basePoint {
public:
  basePoint() = default;
  virtual ~basePoint() = 0;

  T getCoord(uint32_t coordinate) { return m_coordArray[coordinate]; }

  T get(coord coordinate) { return m_coordArray[coordinate]; }

protected:
  std::array<T, n> m_coordArray;
};

// nth dimensional point expressed in integer coordinates
template <int n> class Point : public basePoint<int, n> {
public:
  using funni::basePoint<int, n>::m_coordArray;

  Point(const int (&&coordArray)[n]) { m_coordArray = coordArray; }
};

// nth dimensional point expressed in positive integer coordinates (i.e.
// quadrant 1 in a 2D space)
template <int n> class PosPoint : public basePoint<uint32_t, n> {
public:
  using funni::basePoint<uint32_t, n>::m_coordArray;

  PosPoint(const int (&&coordArray)[n]) { m_coordArray = coordArray; }
};

// nth dimension point expressed in integer coordinates, constrained to a
// bounded positive space by finding the remainder of each input coordinate's
// value when divided by it's prescribed bound as if the point had "wrapped"
// around as many times as necessary to fall within the boundary. Coordinates
// wrap in the direction indicated by their sign, so a wrapped negative point is
// not the absolute value of the negative remainder, but the sum of the negative
// remainder and the boundary size. I.e. on an axis bounded to size 5 (indexes
// 0-4): coord -2 would wrap around to coord 3
template <int n> class WrappedPoint : public basePoint<uint32_t, n> {

public:
  using funni::basePoint<uint32_t, n>::m_coordArray;

  WrappedPoint(std::array<int, n> rpoint, const uint32_t (&bound)[n]) {
    for (int i = 0; i < n; i++) {
      rpoint[i] = rpoint[i] % bound[i];
      m_coordArray[i] = (rpoint[i] + bound[i]) % bound[i];
    }
  }

  // WrappedPoint(Point<n> point, PosPoint<n> bound);

  ~WrappedPoint() = default;

private:
};

} // namespace funni
