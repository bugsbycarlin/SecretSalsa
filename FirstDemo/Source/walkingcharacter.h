/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <deque>
#include <map>
#include <string>
#include <vector>

#include "honey.h"
#include "character.h"

using namespace Honey;
using namespace std;

class WalkingCharacter : public Character {
 public:
  WalkingCharacter(State* state);

  ~WalkingCharacter();

  void walkBehavior(float ax, float ay);
  void followBehavior(int x, int y);
  void seekBehavior(int x, int y);
  void koBehavior();

  void draw();

  float vx;
  float vy;

  deque<position> position_history;
  const int history_size = 80;

  const float velocity_tolerance = 45;
  const float restitution = 0.85;

  // No longer const, set in constructor for now
  float max_velocity;
  float max_ax;
  float max_ay;
  float velocity_decay;
};