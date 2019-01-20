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

class Walkin : public Mode {
 public:
  Walkin();

  void initialize();
  void logic();
  void render();

  ~Walkin();
};