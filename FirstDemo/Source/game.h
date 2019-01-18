/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "honey.h"

using namespace Honey;
using namespace std;

class Game : public Screen {
 public:
  Game();

  void loop();

  void initialize();
  void loadPath();

  void logic();
  bool checkPath(int x, int y);

  void render();

  ~Game();

  std::string screen_color;

  int screen_width;
  int screen_height;

  string map_name;
  int map_width;
  int map_height;

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

  float running_animation_speed;
  int frame;

  int camera_x;
  int camera_y;
  int camera_target_x;
  int camera_target_y;
  const float camera_blend_factor = 0.2;

  struct map_circle {
    int x;
    int y;
    int r;
  };

  vector<map_circle> path;
};