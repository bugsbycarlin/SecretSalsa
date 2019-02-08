/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>
#include <vector>

#include "honey.h"
#include "walkingcharacter.h"
#include "state.h"

using namespace Honey;
using namespace std;

class WalkingParty {
 public:
  WalkingParty(State* state);

  void add(WalkingCharacter* character);
  void walk(float ax, float ay);
  void seek(int x, int y);
  void draw();

  ~WalkingParty();

  State* state;

  vector<WalkingCharacter*> characters;
  vector<WalkingCharacter*> draw_order_characters;

  int walk_trailing_distance;

 private:
  void followLeader();
};