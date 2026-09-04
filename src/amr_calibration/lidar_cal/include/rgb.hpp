#pragma once

#include <cstdint>
#include <cstdlib>
#include <iostream>

class RGB
{
public:
  uint8_t r;
  uint8_t g;
  uint8_t b;
  RGB(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
  static RGB random()
  {
    int r = rand() % 256; // Generate a random value for red (0-255)
    int g = rand() % 256; // Generate a random value for green (0-255)
    int b = rand() % 256; // Generate a random value for blue (0-255)
    return RGB(r, g, b);
  }
};

inline std::ostream &operator<<(std::ostream &out, RGB const &rgb)
{
  out << "(" << rgb.r << ", " << rgb.g << ", " << rgb.b << ")";
  return out;
}
