/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>

#include "honey.h"
#include "mode.h"

using namespace Honey;
using namespace std;

class Kitchen : public Mode {
 public:
  Kitchen();

  void initialize();
  void logic();
  void render();

  ~Kitchen();
};