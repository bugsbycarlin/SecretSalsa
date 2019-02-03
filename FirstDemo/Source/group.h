/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>
#include <vector>

#include "honey.h"
#include "character.h"
#include "state.h"

using namespace Honey;
using namespace std;

class Group {
 public:
  Group(State* state);

  void add(Character* character);
  void walk(float ax, float ay);
  void seek(int x, int y);
  void draw();

  ~Group();

  State* state;

  vector<Character*> characters;
  vector<Character*> draw_order_characters;

  int walk_trailing_distance;

 private:
  void followLeader();
};