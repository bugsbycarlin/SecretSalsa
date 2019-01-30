/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>

#include "honey.h"
#include "mode.h"
#include "battlin.h"

using namespace Honey;
using namespace std;

class Walkin : public Mode {
 public:
  Walkin(State* state);

  void initialize();
  void logic();
  void render();

  void movementLogic();
  void animationLogic();
  void cameraLogic();
  void gameLogic();

  bool checkBox(int x, int y, box b);

  ~Walkin();

  int coach_bulldog_x;
  int coach_bulldog_y;

  int player_direction;
  int player_x;
  int player_y;
  int player_margin_x;
  int player_margin_y;
  float player_vx;
  float player_vy;
  const float player_max_velocity = 7.0;
  const float player_ax = 0.75;
  const float player_ay = 0.65;
  const float player_velocity_decay = 0.9;

  const float velocity_tolerance = 45;
  const float restitution = 0.85;

  float running_animation_speed;
  int frame;

  Textbox* hp_box;
  Textbox* laps_box;

  // Menu* character_menu;
  // Menu* conversation_menu;

  vector<box> lap_zones;
  int lap_zone_counter;
};