/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>

#include <boost/filesystem.hpp>

#include "honey.h"
#include "mode.h"

using namespace Honey;
using namespace std;
using namespace boost::filesystem;

class WorldBuilder;

class ModeChoosingMap : public Mode {
 public:
  ModeChoosingMap(WorldBuilder* worldbuilder);

  void initialize();
  void logic();
  void render();

  void loadMap(string map_name);

  ~ModeChoosingMap();

  Menu* choice_box;

  int choice_value;

  vector<string> map_choices;

  WorldBuilder* worldbuilder;
};