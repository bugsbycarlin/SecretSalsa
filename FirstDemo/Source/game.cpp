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
  state->map->logic();
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
  state->store("laps", 0);

  graphics.addImages("Art/", {
    "baddie_static",
    "baddie_walk_1",
    "baddie_walk_2",
    "baddie_ko",
    "coach_bulldog",
    "coach_bulldog_profile",
    "tune_bear",
    "robin",
    "robin_flapping",
    "witchycat",
    "tune_bear_profile",
    "choice_arrow"
  });

  //state->modes.push(new Talkin(state));
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
