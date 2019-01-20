/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "game.h"

using namespace std;
using namespace Honey;

Game::Game() {
  state = new State();
}

void Game::loop() {
  logic();
  state->modes.top()->logic();

  render();
  state->modes.top()->render();
}

void Game::initialize() {
  state->screen_color = hot_config.getString("layout", "screen_color");
  state->screen_width = hot_config.getInt("layout", "screen_width");
  state->screen_height = hot_config.getInt("layout", "screen_height");

  state->map = new Map(hot_config.getString("layout", "map_name"));

  state->player_hp = 1000;

  graphics.addImages("Art/", {
    "Character_Test_Frame_1",
    "Character_Test_Frame_2",
    "Character_Test_Frame_3",
    "Character_Test_Frame_1_flip",
    "Character_Test_Frame_2_flip",
    "Character_Test_Frame_3_flip",
  });

  state->modes.push(new Walkin(state));
  state->modes.top()->initialize();
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
  graphics.clearScreen(state->screen_color);

  // Switch to 2D drawing mode
  graphics.draw2D();
}

Game::~Game() {
  delete state;
}
