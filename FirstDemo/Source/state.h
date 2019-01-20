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

using namespace Honey;
using namespace std;

class State {
 public:
  State();

  ~State();

  stack<Mode> modes;

  std::string screen_color;

  int screen_width;
  int screen_height;
};