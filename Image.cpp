#include "Image.hpp"

namespace prog
{
  Image::Image(int w, int h, const Color &fill) :
    wdt(w), hgt(h)
  {
    data = new Color[wdt * hgt];

    for(int i=0; i<wdt*hgt; i++){
      data[i]=fill;
    }
  }
  Image::~Image()
  {
    delete[] data;
  }
  int Image::width() const
  {
    return wdt;
  }
  int Image::height() const
  {
    return hgt;
  }

  Color& Image::at(int x, int y)
  {
    int index = y * wdt + x;
    return data[index];
  }

  const Color& Image::at(int x, int y) const
  {
    int index = y * wdt + x;
    return data[index];
  }
}
