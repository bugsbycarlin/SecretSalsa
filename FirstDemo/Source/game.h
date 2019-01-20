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
#include "walkin.h"
#include "talkin.h"
#include "battlin.h"
#include "map.h"
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

  Map* map;

  stack<Mode*> modes;

  std::string screen_color;

  int screen_width;
  int screen_height;

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

  int num_checked;

  int camera_x;
  int camera_y;
  int camera_target_x;
  int camera_target_y;
  const float camera_blend_factor = 0.2;
};