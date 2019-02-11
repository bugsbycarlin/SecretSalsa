/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>

#include "honey.h"
#include "mode.h"
#include "conversation.h"
#include "walkin.h"

using namespace Honey;
using namespace std;

class Options : public Mode {
 public:
  Options(State* state);

  void initialize();
  void logic();
  void render();

  ~Options();
};