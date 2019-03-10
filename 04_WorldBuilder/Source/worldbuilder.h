/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <array>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "nfd.h"

#include "honey.h"
#include "drawable.h"
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

  void generalLogic();
  void menuLogic();
  void cameraLogic();
  void layerLogic();
  void toolLogic();
  void placementLogic();
  void selectionLogic();

  void setMenuNewOrLoad();
  void setMenuAdd();

  void newMasterFileDialog();
  void loadMasterFileDialog();
  void initalizeFromMasterFile();

  void loadNewPictureDialog();

  void updateSelectionOverlay();

  ~WorldBuilder();

  Map* map;

  string current_mode;
  string current_sub_mode;

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

  string placement_image;
  int placement_cursor_x;
  int placement_cursor_y;

  std::map<int, vector<Drawable*>> layers; 

  int layer_value;

  Textbox* camera_text;
  Textbox* layer_text;

  Textbox* place_with_mouse_text;
  Textbox* place_with_keyboard_text;

  Textbox* current_info_text;

  int choice_value;
  int num_choices;
  Menu* choice_box;
  function<void()> choice_action;
  function<void()> cancel_action;

  bool place_images_with_mouse;

  const string MODE_MENU = "MODE_MENU";
  const string MODE_FREE_NAVIGATION = "MODE_FREE_NAVIGATION";
  const string MODE_PLACEMENT = "MODE_PLACEMENT";
  const string MODE_SELECTION = "MODE_SELECTION";
  const string MODE_CALCULATION = "MODE_CALCULATION";


  std::vector<std::vector<int>> pixel_status;

  SDL_Surface* overlay_surface;

  const Uint32 rmask = 0x00ff0000;
  const Uint32 gmask = 0x0000ff00;
  const Uint32 bmask = 0x000000ff;
  const Uint32 amask = 0xff000000;

  int map_width;
  int map_height;

  bool eraser;

  int selection_cursor_x;
  int selection_cursor_y;

  const int max_stamp_radius = 100;
  const int default_stamp_radius = 20;
  int stamp_radius;

  // const string MODE_CHOOSING_MAP = "mode_choosing_map";
  // const string MODE_SELECTING = "mode_selecting";
  // const string MODE_TESTING = "mode_testing";
};