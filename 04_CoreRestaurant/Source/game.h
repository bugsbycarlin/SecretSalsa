/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>

#include "honey.h"

#include "mode.h"
#include "kitchen.h"

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

  map<string, Mode*> modes;
  Mode* current_mode;
};