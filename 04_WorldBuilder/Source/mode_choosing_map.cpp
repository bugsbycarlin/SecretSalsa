/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "mode_choosing_map.h"

using namespace std;
using namespace Honey;

ModeChoosingMap::ModeChoosingMap(WorldBuilder* worldbuilder) {
  this->worldbuilder = worldbuilder;
}

void ModeChoosingMap::initialize() {
  graphics.addImages("Art/", {
    "choice_arrow"
  });

  map_choices = {};
  for (directory_iterator itr(hot_config.getString("system", "map_root_folder")); itr != directory_iterator{}; ++itr) {
    if (!is_regular_file(itr->path())) {
      map_choices.push_back(itr->path().stem().c_str());
      printf("%s\n", itr->path().filename().c_str());
    }
  }

  choice_box = new Menu("Art/", "menu", "choice");
  choice_box->setTextLines(map_choices);
  choice_value = 0;
}

void ModeChoosingMap::logic() {
  if (input.actionPressed("up") > 0) {
    sound.playSound("select_sound", 1);
    choice_value -= 1;
    if (choice_value < 0) {
      choice_value = map_choices.size() - 1;
    }
  }

  if (input.actionPressed("down") > 0) {
    sound.playSound("select_sound", 1);
    choice_value += 1;
    if (choice_value > map_choices.size() - 1) {
      choice_value = 0;
    }
  }

  if (input.actionPressed("accept") > 0) {
    sound.playSound("accept_sound", 1);
    //loadMap(map_choices[choice_value]);
  }
}

void ModeChoosingMap::render() {
  graphics.setLayer(1.0);
  choice_box->draw();

  graphics.drawImage(
    "choice_arrow",
    hot_config.getInt("menu", "choice_x") + hot_config.getInt("menu", "choice_margin_x") - 30,
    hot_config.getInt("menu", "choice_y") + hot_config.getInt("menu", "choice_margin_y") + 4 + 28 * choice_value
  );
}

ModeChoosingMap::~ModeChoosingMap() {
  delete choice_box;
}