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
#include "state.h"

using namespace Honey;
using namespace std;

class Character {
 public:
  Character(State* state);

  virtual void addAnimation(string name, vector<string> animation);
  virtual void setAnimation(string animation);
  virtual void setFrame(int frame);

  void walkBehavior(float ax, float ay);
  void followBehavior(int x, int y);
  void seekBehavior(int x, int y);

  void simpleBounceAnimation();
  virtual void walkAnimation();

  virtual void draw();

  ~Character();

  State* state;

  map<string, vector<string>> animations;
  string current_animation;
  int current_frame;

  float walk_animation_speed;

  int direction;
  int x;
  int y;
  int margin_x;
  int margin_y;
  float vx;
  float vy;

  deque<position> position_history;
  const int history_size = 80;

  const float velocity_tolerance = 45;
  const float restitution = 0.85;

  // const float max_velocity = 7.0;
  // const float max_ax = 0.75;
  // const float max_ay = 0.65;
  // const float velocity_decay = 0.87;
  // No longer const, set in constructor for now
  float max_velocity;
  float max_ax;
  float max_ay;
  float velocity_decay;

  int hp;
  int max_hp;

  int battle_x;
  int battle_y;

  string unique_name;

 protected:
  static int unique_count;
};