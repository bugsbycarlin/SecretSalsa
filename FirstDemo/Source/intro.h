/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>

#include "honey.h"
#include "mode.h"
#include "talkin.h"

using namespace Honey;
using namespace std;

class Intro : public Mode {
 public:
  Intro(State* state);

  void initialize();
  void logic();
  void render();

  ~Intro();
};