/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>

#include "honey.h"
#include "mode.h"
#include "map.h"

using namespace Honey;
using namespace std;

class ModeSelecting : public Mode {
 public:
  ModeSelecting(Map* map);

  void initialize();
  void logic();
  void render();

  void updateOverlay();

  ~ModeSelecting();

  Map* map;

  int map_width;
  int map_height;

  std::vector<std::vector<int>> pixel_status;

  SDL_Surface* overlay_surface;

  const Uint32 rmask = 0x00ff0000;
  const Uint32 gmask = 0x0000ff00;
  const Uint32 bmask = 0x000000ff;
  const Uint32 amask = 0xff000000;

  int camera_x;
  int camera_y;
  float camera_vx;
  float camera_vy;
  const float camera_velocity_decay = 0.95;

  bool eraser;

  int cursor_x;
  int cursor_y;

  const int max_stamp_radius = 100;
  const int default_stamp_radius = 20;
  int stamp_radius;
};