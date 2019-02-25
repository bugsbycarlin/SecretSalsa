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

class Talkin : public Mode {
 public:
  Talkin(State* state);

  void initialize();
  void logic();
  void render();

  ~Talkin();

  int coach_bulldog_x;
  int coach_bulldog_y;

  int tune_bear_x;
  int tune_bear_y;

  Conversation* conversation;

  Menu* character_menu;
  Menu* conversation_menu;
};