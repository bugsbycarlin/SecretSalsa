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

  layer_value = 0;
  layers[0] = {};

  place_images_with_mouse = false;
}

void WorldBuilder::loop() {
  logic();
  render();
}

void WorldBuilder::initialize() {

  map_width = 2000;
  map_height = 1500;

  if (overlay_surface != NULL) {
    SDL_FreeSurface(overlay_surface);
  }
  overlay_surface = SDL_CreateRGBSurface(0, map_width, map_height, 32, rmask, gmask, bmask, amask);
  graphics.addImageFromSurface("selection_overlay", overlay_surface);

  pixel_status.resize(map_width);
  for (int k = 0; k < map_width; k++) {
    pixel_status[k].resize(map_height);
  }

  for (int k = 0; k < map_width; k++) {
    for (int l = 0; l < map_height; l++) {
      pixel_status[k][l] = 0;
    }
  }

  stamp_radius = default_stamp_radius;

  eraser = false;

  selection_cursor_x = -1000;
  selection_cursor_y = -1000;

  input.addActionKey("up", "up");
  input.addActionKey("down", "down");
  input.addActionKey("left", "left");
  input.addActionKey("right", "right");
  input.addActionKey("accept", "return");
  input.addActionKey("cancel", "escape");

  input.addActionKey("camera_2", "right_command");
  input.addActionKey("camera", "left_command");

  input.addActionKey("layer_down", "-");
  input.addActionKey("layer_up", "=");

  input.addActionKey("stamp_radius_down", "-");
  input.addActionKey("stamp_radius_up", "=");

  input.addActionKey("add", "a");
  input.addActionKey("double", "d");
  input.addActionKey("grass", "g");

  input.addActionKey("fast_move", "left_alt");
  input.addActionKey("fast_move_2", "right_alt");

  input.addActionKey("use_mouse", "m");
  input.addActionKey("use_keyboard", "k");

  input.addActionKey("erase", "e");

  graphics.addImages("Art/", {
    "waiting_wheel_0",
    "waiting_wheel_1",
    "grid_lines",
    "origin_x",
    "origin_o",
    "choice_arrow",
    "cursor",
    "eraser",
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

  place_with_mouse_text = new Textbox(
    "Fonts/avenir_black.ttf",
    20,
    "Place images with (M)ouse",
    (position) {20, 660},
    "#000000"
  );

  place_with_keyboard_text = new Textbox(
    "Fonts/avenir_black.ttf",
    20,
    "Place images with (K)eyboard",
    (position) {20, 680},
    "#000000"
  );

  current_info_text = new Textbox(
    "Fonts/avenir_black.ttf",
    20,
    "X",
    (position) {680, 680},
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
    current_sub_mode = "";
    if (choice_value == 0) {
      newMasterFileDialog();
    } else if (choice_value == 1) {
      loadMasterFileDialog();
    }
  };
  cancel_action = [this]() {
    setMenuNewOrLoad();
  };
  current_sub_mode = "first";
  current_mode = MODE_MENU;
}

void WorldBuilder::setMenuAdd() {
  choice_box->setTextLines(pictures);
  choice_value = 0;
  num_choices = pictures.size();
  choice_action = [this]() {
    if (choice_value == 0) {
      loadNewPictureDialog();
    } else {

      graphics.addImage(pictures[choice_value], picture_paths[choice_value]);
      placement_image = pictures[choice_value];
      placement_cursor_x = 640 + camera_x;
      placement_cursor_y = 360 + camera_y;

      //loadMasterFileDialog();
      current_mode = MODE_PLACEMENT;
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
    string new_picture_path(outPath);
    boost::filesystem::path path(new_picture_path);
    string new_picture_name(path.stem().c_str());
    pictures.push_back(new_picture_name);
    picture_paths.push_back(new_picture_path);
    setMenuAdd();
    input.lockInput(0.3);
  }
  else if (result == NFD_CANCEL) {
    puts("User pressed cancel.");
  }
  else {
    printf("Error: %s\n", NFD_GetError());
    exit(1);
  }
}

void WorldBuilder::updateSelectionOverlay() {
  SDL_LockSurface(overlay_surface);
  unsigned int *overlay_ptr = (unsigned int*)overlay_surface->pixels;

  for (int k = 0; k < map_width; k++) {
    for (int l = 0; l < map_height; l++) {
      int overlay_offset = l * overlay_surface->w;
      Uint32 mask = 0x77111111;
      if (pixel_status[k][l] == 1) {
        mask = 0x00111111;
      } else if (pixel_status[k][l] == 2) {
        mask = 0x44111111;
      }

      overlay_ptr[overlay_offset + k] = mask;
    }
  }

  SDL_UnlockSurface(overlay_surface);

  graphics.removeImage("selection_overlay");
  graphics.addImageFromSurface("selection_overlay", overlay_surface);
}

void WorldBuilder::logic() {
  if (input.keyPressed("quit") > 0) {
    screenmanager.setQuit();
  }

  if (input.threeQuickKey("escape")) {
    screenmanager.setQuit();
  }

  generalLogic();
  if (current_mode != MODE_SELECTION) {
    layerLogic();
  }

  if (current_mode == MODE_MENU) {
    menuLogic();
  } else if (current_mode != MODE_MENU) {
    cameraLogic();
    
    if (current_mode == MODE_FREE_NAVIGATION) {
      toolLogic();
    } else if (current_mode == MODE_PLACEMENT) {
      placementLogic();
    } else if (current_mode == MODE_SELECTION) {
      selectionLogic();
    }
  }
}

void WorldBuilder::cameraLogic() {
  if ((input.actionDown("camera") || input.actionDown("camera_2")) && input.actionDown("left")) {
    camera_vx -= 1;
  }
  if ((input.actionDown("camera") || input.actionDown("camera_2")) && input.actionDown("right")) {
    camera_vx += 1;
  }
  if ((input.actionDown("camera") || input.actionDown("camera_2")) && input.actionDown("up")) {
    camera_vy -= 1;
  }
  if ((input.actionDown("camera") || input.actionDown("camera_2")) && input.actionDown("down")) {
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

void WorldBuilder::generalLogic() {
  if (input.actionPressed("use_mouse") > 0) {
    place_images_with_mouse = true;
  }

  if (input.actionPressed("use_keyboard") > 0) {
    place_images_with_mouse = false;
  }
}

void WorldBuilder::layerLogic() {
  if (input.actionPressed("layer_up") > 0) {
    sound.playSound("select_sound", 1);
    layer_value += 1;
    if (layers.count(layer_value) != 1) {
      layers[layer_value] = {};
    }
  }

  if (input.actionPressed("layer_down") > 0) {
    sound.playSound("select_sound", 1);
    layer_value -= 1;
    if (layers.count(layer_value) != 1) {
      layers[layer_value] = {};
    }
  }
}

void WorldBuilder::toolLogic() {
  if (input.actionPressed("add") > 0) {
    sound.playSound("accept_sound", 1);
    setMenuAdd();
  } else if (input.actionPressed("double") > 0) {
    if (placement_image != "") {
      placement_cursor_x = 640 + camera_x;
      placement_cursor_y = 360 + camera_y;

      //loadMasterFileDialog();
      current_mode = MODE_PLACEMENT;
    }
  } else if (input.actionPressed("grass") > 0) {
    updateSelectionOverlay();
    current_sub_mode = "grass";
    current_mode = MODE_SELECTION;
  }
}

void WorldBuilder::placementLogic() {
  if (place_images_with_mouse) {
    vector<mouseEvent> mouse_events = input.getMouseEvents();
    for (mouseEvent event : mouse_events) {
      placement_cursor_x = event.x + camera_x;
      placement_cursor_y = event.y + camera_y;
      if (event.button == "left") {
        int x = event.x + camera_x;
        int y = event.y + camera_y;
        layers[layer_value].push_back(new Drawable(placement_image, x, y));
        current_mode = MODE_FREE_NAVIGATION;
      }
    }
  } else {
    if (input.actionDown("camera") || input.actionDown("camera_2")) {
      return;
    }
    int move_scale = 1;
    if (input.actionDown("fast_move") || input.actionDown("fast_move_2")) {
      move_scale = 10;
    }
    if (input.actionDown("left")) {
      placement_cursor_x -= move_scale;
    }
    if (input.actionDown("right")) {
      placement_cursor_x += move_scale;
    }
    if (input.actionDown("up")) {
      placement_cursor_y -= move_scale;
    }
    if (input.actionDown("down")) {
      placement_cursor_y += move_scale;
    }

    if (input.actionPressed("accept") > 0) {
      layers[layer_value].push_back(new Drawable(placement_image, placement_cursor_x, placement_cursor_y));
      current_mode = MODE_FREE_NAVIGATION;
    }
  }
}

void WorldBuilder::selectionLogic() {
  vector<mouseEvent> mouse_events = input.getMouseEvents();
  for (mouseEvent event : mouse_events) {
    selection_cursor_x = event.x;
    selection_cursor_y = event.y;
    if (event.button == "left") {
      int x = event.x + camera_x;
      int y = event.y + camera_y;
      for (int k = x - stamp_radius; k < x + stamp_radius; k++) {
        for (int l = y - stamp_radius; l < y + stamp_radius; l++) {
          if (k >= 0 && k < map_width && l >= 0 && l < map_height) {
            if (math_utils.distance(x,y,k,l) <= stamp_radius) {
              pixel_status[k][l] = 1;
              if (eraser) pixel_status[k][l] = 0;
            }
          }
        }
      }
      updateSelectionOverlay();
    }
  }

  if (input.actionPressed("erase") > 0) {
    eraser = !eraser;
  }

  if (input.keyPressed("=") > 0) {
    if (stamp_radius <= max_stamp_radius) stamp_radius += 5;
  }

  if (input.keyPressed("-") > 0) {
    if (stamp_radius > 5) stamp_radius -= 5;
  }
}

void WorldBuilder::render() {
  // Clear the screen to a soft white color
  graphics.clearScreen("#808080");
  graphics.setLayer(0.0);
  graphics.setColor("#FFFFFF", 1.0);

  if (current_sub_mode != "first") {
    graphics.useOrderBasedLayers();
    graphics.setLayer(-40.0);
    for (int i = -1; i < 14; i++) {
      for (int j = -1; j < 9; j++) {
        graphics.drawImage("grid_lines", 100 * i - camera_x % 100, 100 * j - camera_y % 100);
      }
    }
    graphics.drawImage("origin_x", -camera_x, -camera_y, true, 0, 1);
    graphics.drawImage("origin_o", 0, 0, true, 0, 1);
    
    camera_text->setText(to_string(camera_x) + "," + to_string(camera_y));
    camera_text->draw();

    graphics.setColor("#FFFFFF", 1.0);
    graphics.usePositionBasedLayers();
    for (std::pair<int, vector<Drawable*>> element : layers) {
      int current_layer = element.first;
      vector<Drawable*> items = element.second;
      graphics.setLayer(current_layer);
      for (Drawable* item : items) {
        item->draw(camera_x, camera_y);
      }
    }

    graphics.useOrderBasedLayers();
    if (current_mode == MODE_PLACEMENT) {
      graphics.setLayer(39.0);
      graphics.setColor("#FFFFFF", 0.4);
      graphics.drawImage(placement_image, placement_cursor_x - camera_x, placement_cursor_y - camera_y, true, 0, 1);
      graphics.setColor("#FFFFFF", 1.0);
    }

    if (current_mode == MODE_SELECTION) {
      graphics.setLayer(39.0);
      graphics.drawImage("selection_overlay", -camera_x, -camera_y);

      graphics.drawImage((eraser) ? "eraser" : "cursor", selection_cursor_x, selection_cursor_y, true, 0, stamp_radius / 50.0);
    }

    graphics.setLayer(40.0);

    if (place_images_with_mouse) {
      place_with_mouse_text->setColor("#000000");
      place_with_keyboard_text->setColor("#777777");
    } else {
      place_with_mouse_text->setColor("#777777");
      place_with_keyboard_text->setColor("#000000");
    }
    place_with_mouse_text->draw();
    place_with_keyboard_text->draw();
    
    layer_text->setText("Layer " + to_string(layer_value));
    layer_text->draw();

    if (current_mode == MODE_FREE_NAVIGATION) {
      current_info_text->setText("Navigate using Cmd-Left, Cmd-Right, Cmd-Up, Cmd-Down");
    } else if (current_mode == MODE_PLACEMENT) {
      current_info_text->setText("Placing image at " + to_string(placement_cursor_x) + "," + to_string(placement_cursor_y));
    } else if (current_mode == MODE_SELECTION && current_sub_mode == "grass") {
      string grass_text = eraser ? "Select a region to grassify. Press (E) for Stamp" : "Select a region to grassify. Press (E) for Eraser";
      current_info_text->setText(grass_text);
    } else if (current_mode == MODE_CALCULATION && current_sub_mode == "grass") {
      current_info_text->setText("Grassifying region...");
    } else {
      current_info_text->setText(" ");
    }
    current_info_text->draw();
  }
  

  if (current_mode == MODE_MENU) {
    graphics.useOrderBasedLayers();
    graphics.setLayer(40.0);
    choice_box->draw();

    graphics.drawImage(
      "choice_arrow",
      hot_config.getInt("menu", "choice_x") + hot_config.getInt("menu", "choice_margin_x") - 30,
      hot_config.getInt("menu", "choice_y") + hot_config.getInt("menu", "choice_margin_y") + 4 + 28 * choice_value
    );
  }
}

WorldBuilder::~WorldBuilder() {

}
