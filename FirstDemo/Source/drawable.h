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

  virtual void draw(int x, int y);

  virtual ~Drawable();
};
