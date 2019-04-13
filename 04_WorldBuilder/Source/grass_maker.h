/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "honey.h"
#include "drawable.h"

using namespace Honey;
using namespace std;

class GrassMaker {
 public:
  GrassMaker(std::vector<std::vector<int>> selection_pixels, function<void(int)> progress_action);

  ~GrassMaker();

  Drawable* result;

  vector<float> wind_frames; // {-0.5, -0.25, 0, 0.25, 0.5, 0.625, 0.75, 0.875, 1.0};
  vector<SDL_Surface*> windy_surfaces;

  const Uint32 rmask = 0x00ff0000;
  const Uint32 gmask = 0x0000ff00;
  const Uint32 bmask = 0x000000ff;
  const Uint32 amask = 0xff000000;

  static int unique_count;
};