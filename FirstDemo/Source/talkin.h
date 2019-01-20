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

class Talkin : public Mode {
 public:
  Talkin();

  void initialize();
  void logic();
  void render();

  ~Talkin();
};