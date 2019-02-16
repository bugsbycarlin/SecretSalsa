/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <array>
#include <stack>
#include <string>
#include <vector>

#include "honey.h"

#include "mode.h"
#include "intro.h"
#include "walkin.h"
#include "talkin.h"
#include "battlin.h"
#include "state.h"

using namespace Honey;
using namespace std;

class Game : public Screen {
 public:
  Game();

  void loop();

  void initialize();

  void logic();

  void render();

  ~Game();

  State* state;
};