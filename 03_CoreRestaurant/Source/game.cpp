/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "game.h"

using namespace std;
using namespace Honey;

Game::Game() {
}

void Game::loop() {
  logic();
  current_mode->logic();

  render();
  current_mode->render();
}

void Game::initialize() {
  input.addActionKey("up", "up");
  input.addActionKey("down", "down");
  input.addActionKey("left", "left");
  input.addActionKey("right", "right");
  input.addActionKey("accept", "a");
  input.addActionKey("cancel", "s");

  // graphics.addImages("Art/", {
  //   "intro_screen",
  // });

  // layouts.makeTileLayout(
  //   "battle_left_layout",
  //   hot_config.getInt("layout", "battle_left_layout_x"),
  //   hot_config.getInt("layout", "battle_left_layout_y"),
  //   hot_config.getInt("layout", "battle_left_layout_x_margin"),
  //   hot_config.getInt("layout", "battle_left_layout_y_margin")
  // );

  // sound.addMusics("Music/", {
  //   "crepuscolo_su_saturno",
  // });

  // sound.addSounds("Sound/", {
  //   "select_sound",
  //   "accept_sound",
  //   "cancel_sound",
  // });

  // if (hot_config.getString("game", "starting_screen") == "talking") {
  //   state->modes.push(new Talkin(state));
  // } else if (hot_config.getString("game", "starting_screen") == "walking") {
  //   state->modes.push(new Walkin(state));
  // } else if (hot_config.getString("game", "starting_screen") == "intro") {
  //   state->modes.push(new Intro(state));
  // }

  modes["kitchen"] = new Kitchen();
  current_mode = modes["kitchen"];
  
  current_mode->initialize();
}

void Game::logic() {
  if (input.keyPressed("quit") > 0) {
    screenmanager.setQuit();
  }

  if (input.threeQuickKey("escape")) {
    screenmanager.setQuit();
  }
}

void Game::render() {
  // Clear the screen to a soft white color
  graphics.clearScreen("#555555");

  // Switch to 2D drawing mode
  graphics.draw2D();
}

Game::~Game() {
}
