/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>

#include "honey.h"
#include "mode.h"
#include "character.h"

using namespace Honey;
using namespace std;

class Battlin : public Mode {
 public:
  struct Placement {
    Character* character;
    int i;
    int j;
  };

  Battlin(State* state);

  void initialize();
  void logic();
  void render();

  ~Battlin();

  Textbox* hp_box;

  vector<Placement> left_placements;
  vector<Placement> right_placements;
};