/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "worldbuilder.h"

using namespace std;
using namespace Honey;

void debug(int x) {
  printf("Here %d\n", x);
}

WorldBuilder::WorldBuilder() {
  map = NULL;

  camera_x = 0;
  camera_y = 0;

  camera_vx = 0;
  camera_vy = 0;
}

void WorldBuilder::loop() {
  logic();

  //modes[current_mode]->logic();

  render();

  //modes[current_mode]->render();
}

void WorldBuilder::initialize() {
  // state->screen_color = hot_config.getString("layout", "screen_color");
  // state->screen_width = hot_config.getInt("layout", "screen_width");
  // state->screen_height = hot_config.getInt("layout", "screen_height");

  //map = new Map(hot_config.getString("layout", "map_name"));


  input.addActionKey("up", "up");
  input.addActionKey("down", "down");
  input.addActionKey("left", "left");
  input.addActionKey("right", "right");
  input.addActionKey("accept", "a");
  input.addActionKey("cancel", "s");

  input.addActionKey("camera2", "right_command");
  input.addActionKey("camera", "left_command");

  input.addActionKey("layer_down", "-");
  input.addActionKey("layer_up", "=");

  input.addActionKey("add", "a");

  graphics.addImages("Art/", {
    "waiting_wheel_0",
    "waiting_wheel_1",
    "grid_lines",
    "origin_x",
    "origin_o",
    "choice_arrow",
    "tune_bear",
    "robin",
    "witchycat"
  });

  sound.addSounds("Sound/", {
    "select_sound",
    "accept_sound",
    "cancel_sound",
    "thunder",
  });

  camera_text = new Textbox(
    "Fonts/avenir_black.ttf",
    20,
    "X",
    (position) {20, 20},
    "#777777"
  );

  layer_text = new Textbox(
    "Fonts/avenir_black.ttf",
    20,
    "Layer 0",
    (position) {1180, 20},
    "#000000"
  );

  pictures = {"Add New"};
  picture_paths = {"_"};

  choice_box = new Menu("Art/", "menu", "choice");

  setMenuNewOrLoad();
}

void WorldBuilder::setMenuNewOrLoad() {
  choice_box->setTextLines({"New", "Load"});
  choice_value = 0;
  num_choices = 2;
  choice_action = [this]() {
    first_load = false;
    if (choice_value == 0) {
      newMasterFileDialog();
    } else if (choice_value == 1) {
      loadMasterFileDialog();
    }
  };
  cancel_action = [this]() {
    setMenuNewOrLoad();
  };
  first_load = true;
  current_mode = MODE_MENU;
}

void WorldBuilder::setMenuAdd() {
  choice_box->setTextLines(pictures);
  choice_value = 0;
  num_choices = pictures.size();
  choice_action = [this]() {
    if (choice_value == 0) {
      loadNewPictureDialog();
    } else if (choice_value == 1) {
      //loadMasterFileDialog();
      current_mode = MODE_FREE_NAVIGATION;
    }
  };
  cancel_action = [this]() {
    current_mode = MODE_FREE_NAVIGATION;
  };
  current_mode = MODE_MENU;
}

void WorldBuilder::newMasterFileDialog() {
  printf("Making new master file\n");
  nfdchar_t *savePath = NULL;
  nfdresult_t result = NFD_SaveDialog("world", NULL, &savePath);
  if (result == NFD_OKAY) {
    master_file_path.assign(savePath);
    boost::filesystem::path path(master_file_path);
    master_file_name.assign(path.filename().c_str());
    master_file_folder.assign(path.parent_path().c_str());
    free(savePath);
    current_mode = MODE_FREE_NAVIGATION;
  }
  else if (result == NFD_CANCEL) {
    puts("User pressed cancel.");
  }
  else {
    printf("Error: %s\n", NFD_GetError());
    exit(1);
  }
}

void WorldBuilder::loadMasterFileDialog() {
  printf("Loading master file\n");
  nfdchar_t *outPath = NULL;
  nfdresult_t result = NFD_OpenDialog("world", NULL, &outPath);
  if (result == NFD_OKAY) {
    master_file_path.assign(outPath);
    boost::filesystem::path path(master_file_path);
    master_file_name.assign(path.filename().c_str());
    master_file_folder.assign(path.parent_path().c_str());
    free(outPath);
    initalizeFromMasterFile();
    current_mode = MODE_FREE_NAVIGATION;
  }
  else if (result == NFD_CANCEL) {
    puts("User pressed cancel.");
  }
  else {
    printf("Error: %s\n", NFD_GetError());
    exit(1);
  }
}

void WorldBuilder::initalizeFromMasterFile() {

}

void WorldBuilder::loadNewPictureDialog() {
  printf("Loading new picture\n");
  nfdchar_t *outPath = NULL;
  nfdresult_t result = NFD_OpenDialog("png", NULL, &outPath);
  if (result == NFD_OKAY) {
    // master_file_path.assign(outPath);
    // boost::filesystem::path path(master_file_path);
    // master_file_name.assign(path.filename().c_str());
    // master_file_folder.assign(path.parent_path().c_str());
    // free(outPath);
    // initalizeFromMasterFile();

    string new_picture_path(outPath);
    boost::filesystem::path path(new_picture_path);
    string new_picture_name(path.stem().c_str());
    pictures.push_back(new_picture_name);
    picture_paths.push_back(new_picture_path);
    num_choices = pictures.size();
    current_mode = MODE_FREE_NAVIGATION;
  }
  else if (result == NFD_CANCEL) {
    puts("User pressed cancel.");
  }
  else {
    printf("Error: %s\n", NFD_GetError());
    exit(1);
  }
}

void WorldBuilder::setMode(string mode_name, bool create_new) { 
  current_mode = mode_name;

  if (create_new) {
    if (current_mode == MODE_CHOOSING_MAP) {
      modes[current_mode] = new ModeChoosingMap(this);
    } else if (current_mode == MODE_SELECTING) {
      modes[current_mode] = new ModeSelecting(map);
    }

    modes[current_mode]->initialize();
  }
}

void WorldBuilder::logic() {
  if (input.keyPressed("quit") > 0) {
    screenmanager.setQuit();
  }

  if (input.threeQuickKey("escape")) {
    screenmanager.setQuit();
  }

  if (current_mode == MODE_MENU) {
    menuLogic();
  }

  if (current_mode != MODE_MENU) {
    cameraLogic();
    layerLogic();
    toolLogic();
  }
}

void WorldBuilder::cameraLogic() {
  if ((input.actionDown("camera") || input.actionDown("camera2")) && input.actionDown("left")) {
    camera_vx -= 1;
  }
  if ((input.actionDown("camera") || input.actionDown("camera2")) && input.actionDown("right")) {
    camera_vx += 1;
  }
  if ((input.actionDown("camera") || input.actionDown("camera2")) && input.actionDown("up")) {
    camera_vy -= 1;
  }
  if ((input.actionDown("camera") || input.actionDown("camera2")) && input.actionDown("down")) {
    camera_vy += 1;
  }
  camera_x += camera_vx;
  camera_y += camera_vy;
  camera_vx *= camera_velocity_decay;
  camera_vy *= camera_velocity_decay;
}

void WorldBuilder::menuLogic() {
  if (input.actionPressed("up") > 0) {
    sound.playSound("select_sound", 1);
    choice_value -= 1;
    if (choice_value < 0) {
      choice_value = num_choices - 1;
    }
  }

  if (input.actionPressed("down") > 0) {
    sound.playSound("select_sound", 1);
    choice_value += 1;
    if (choice_value > num_choices - 1) {
      choice_value = 0;
    }
  }

  if (input.actionPressed("accept") > 0) {
    sound.playSound("accept_sound", 1);
    choice_action();
  }

  if (input.actionPressed("cancel") > 0) {
    sound.playSound("cancel_sound", 1);
    cancel_action();
  }
}

void WorldBuilder::layerLogic() {
  if (input.actionPressed("layer_up") > 0) {
    sound.playSound("select_sound", 1);
    layer_value += 1;
  }

  if (input.actionPressed("layer_down") > 0) {
    sound.playSound("select_sound", 1);
    layer_value -= 1;
  }
}

void WorldBuilder::toolLogic() {
  if (input.actionPressed("add") > 0) {
    sound.playSound("accept_sound", 1);
    setMenuAdd();
  }
}

void WorldBuilder::loadMap(string map_name) {
  this->map_name = map_name;

  map = new Map(this->map_name);

  setMode(MODE_SELECTING, true);
}

void ModeChoosingMap::loadMap(string map_name) {
  worldbuilder->loadMap(map_name);
}

void WorldBuilder::render() {
  // Clear the screen to a soft white color
  graphics.clearScreen("#808080");
  graphics.setLayer(0.0);

  if (!first_load) {
    for (int i = -1; i < 14; i++) {
      for (int j = -1; j < 9; j++) {
        graphics.drawImage("grid_lines", 100 * i - camera_x % 100, 100 * j - camera_y % 100);
      }
    }
    graphics.drawImage("origin_x", -camera_x, -camera_y, true, 0, 1);
    graphics.drawImage("origin_o", 0, 0, true, 0, 1);
    
    camera_text->setText(to_string(camera_x) + "," + to_string(camera_y));
    camera_text->draw();

    graphics.setLayer(40.0);
    layer_text->setText("Layer " + to_string(layer_value));
    layer_text->draw();
  }

  if (current_mode == MODE_MENU) {
    graphics.setLayer(1.0);
    choice_box->draw();

    graphics.drawImage(
      "choice_arrow",
      hot_config.getInt("menu", "choice_x") + hot_config.getInt("menu", "choice_margin_x") - 30,
      hot_config.getInt("menu", "choice_y") + hot_config.getInt("menu", "choice_margin_y") + 4 + 28 * choice_value
    );
  }


  // graphics.usePositionBasedLayers();

  // for (int i = 0; i < 10; i++) {
  //   //graphics.setLayer((600 - 50 * i));
  //   graphics.drawImage("tune_bear", 300, 300 - 50 * i);
  // }
}

WorldBuilder::~WorldBuilder() {

}
