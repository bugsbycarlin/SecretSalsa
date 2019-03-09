/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <array>
#include <fstream>
#include <stack>
#include <string>
#include <vector>

#include "tinyxml/tinyxml2.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "honey.h"

using namespace Honey;
using namespace std;

class Map {
 public:
  Map(string map_name);

  void loadConfig();
  void loadSheets();
  void drawWaiting();

  string map_name;
  string map_path;

  int scale;

  vector<string> sheet_names;
  vector<string> layer_names;
  vector<string> doodad_names;

  map<string, SDL_Surface*> sheets;

  const Uint32 rmask = 0x00ff0000;
  const Uint32 gmask = 0x0000ff00;
  const Uint32 bmask = 0x000000ff;
  const Uint32 amask = 0xff000000;

  long wait_step;



  void loadPath();
  void initializeRainLayer();

  void startRain();
  void stopRain();
  bool checkPath(int x, int y);

  void logic();

  void draw(int x, int y);
  void overlayer(int x, int y);

  ~Map();
  
  int map_width;
  int map_height;

  bool raining;
  bool ice_shards;
  int dust_storm;

  const float rain_scale = 0.2;
  int rain_density = 150;
  float rain_angle = 10;
  point rain_velocity;
  vector<position> rain_field;
  vector<position> rain_targets;
  vector<point> splash_field;

  vector<position> dust_field;
  vector<position> dust_targets;
  vector<float> dust_orig_x;

  struct map_circle {
    int x;
    int y;
    int r;
  };

  vector<map_circle> path;
  std::array<std::array<vector<map_circle>, 8>, 8> extra_path;
};