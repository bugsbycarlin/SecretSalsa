/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

// Standard libraries
#include <stdio.h>
#include <cstdlib>
#include <string>

#include "honey.h"

using namespace Honey;
using namespace std;

class Drawable {
 public:
  Drawable();

  Drawable(string label, int x, int y);

  virtual void draw(int camera_x, int camera_y);

  virtual ~Drawable();

  string label;
  int x;
  int y;
};
