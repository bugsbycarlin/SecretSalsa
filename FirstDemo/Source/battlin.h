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

class Battlin : public Mode {
 public:
  Battlin(State* state);

  void initialize();
  void logic();
  void render();

  ~Battlin();

  Textbox* hp_box;
};