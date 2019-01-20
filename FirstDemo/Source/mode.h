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

#include "state.h"
#include "map.h"

using namespace Honey;
using namespace std;

class Mode {
 public:
  Mode();

  virtual void initialize();
  virtual void logic();
  virtual void render();

  virtual ~Mode();

  State* state;
};
