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

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "honey.h"
#include "drawable.h"

using namespace Honey;
using namespace std;

class Map : public Drawable {
 public:
  Map(string map_name);

  void loadPath();
  void initializeRainLayer();

  void startRain();
  void stopRain();
  bool checkPath(int x, int y);

  void logic();

  void draw(int x, int y);
  void overlayer(int x, int y);

  ~Map();

  string map_name;
  int map_width;
  int map_height;

  bool raining;

  const float rain_scale = 0.2;
  int rain_density = 150;
  float rain_angle = 10;
  point rain_velocity;
  vector<position> rain_field;
  vector<position> rain_targets;
  vector<point> splash_field;

  struct map_circle {
    int x;
    int y;
    int r;
  };

  vector<map_circle> path;
  std::array<std::array<vector<map_circle>, 8>, 8> extra_path;
};