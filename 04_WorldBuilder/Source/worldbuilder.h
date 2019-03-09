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

#include <boost/filesystem.hpp>

#include "nfd.h"

#include "honey.h"
#include "map.h"
#include "mode.h"
#include "mode_choosing_map.h"
#include "mode_selecting.h"
#include "mode_testing.h"

using namespace Honey;
using namespace std;

class WorldBuilder : public Screen {
 public:
  WorldBuilder();

  void loop();
  void initialize();
  void logic();
  void render();

  void menuLogic();
  void cameraLogic();
  void layerLogic();
  void toolLogic();

  void setMenuNewOrLoad();
  void setMenuAdd();

  void newMasterFileDialog();
  void loadMasterFileDialog();
  void initalizeFromMasterFile();

  void loadNewPictureDialog();

  void setMode(string mode_name, bool create_new);
  void loadMap(string map_name);

  ~WorldBuilder();

  Map* map;

  std::map<string, Mode*> modes;

  string current_mode;

  string master_file_path;
  string master_file_name;
  string master_file_folder;

  string map_name;

  vector<string> pictures;
  vector<string> picture_paths;

  int camera_x;
  int camera_y;
  float camera_vx;
  float camera_vy;
  const float camera_velocity_decay = 0.95;

  int layer_value;

  Textbox* camera_text;
  Textbox* layer_text;

  int choice_value;
  int num_choices;
  Menu* choice_box;
  function<void()> choice_action;
  function<void()> cancel_action;

  bool first_load = false;
  const string MODE_MENU = "MODE_MENU";
  const string MODE_FREE_NAVIGATION = "MODE_FREE_NAVIGATION";


  const string MODE_CHOOSING_MAP = "mode_choosing_map";
  const string MODE_SELECTING = "mode_selecting";
  const string MODE_TESTING = "mode_testing";
};