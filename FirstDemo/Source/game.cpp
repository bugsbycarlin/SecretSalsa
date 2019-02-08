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

  state->store("laps", 0);

  graphics.addImages("Art/", {
    "bigdog_static",
    "bigdog_walk_2_3",
    "bigdog_walk_2_4",
    "bigdog_ko",
    "bigdog_kick",
    "coach_bulldog",
    "coach_bulldog_profile",
    "tune_bear",
    "robin",
    "robin_flapping",
    "witchycat",
    "tune_bear_profile",
    "choice_arrow",
    "choice_arrow_2",
    "choice_hand",
    "charge_gauge_outline",
    "charge_gauge_orange_fill",
    "charge_gauge_green_fill",
    "attack_slash_thick",
    "black_screen"
  });

  PermanentCharacter* p;

  p = new PermanentCharacter(state);
  p->loadFromConfig("tune_bear");
  state->party.push_back(p);

  p = new PermanentCharacter(state);
  p->loadFromConfig("witchycat");
  state->party.push_back(p);

  p = new PermanentCharacter(state);
  p->loadFromConfig("robin");
  state->party.push_back(p);

  p = new PermanentCharacter(state);
  p->loadFromConfig("bigdog");
  state->enemy_templates["bigdog"] = p;

  layouts.makeTileLayout(
    "battle_left_layout",
    hot_config.getInt("layout", "battle_left_layout_x"),
    hot_config.getInt("layout", "battle_left_layout_y"),
    hot_config.getInt("layout", "battle_left_layout_x_margin"),
    hot_config.getInt("layout", "battle_left_layout_y_margin")
  );

  layouts.makeTileLayout(
    "battle_right_layout",
    hot_config.getInt("layout", "battle_right_layout_x"),
    hot_config.getInt("layout", "battle_right_layout_y"),
    hot_config.getInt("layout", "battle_right_layout_x_margin"),
    hot_config.getInt("layout", "battle_right_layout_y_margin")
  );

  sound.addMusics("Music/", {
    "crepuscolo_su_saturno",
    "water",
    "sangue_su_callisto",
    "piano",
    "fog_burning_dawn",
    "battle_fanfare"
  });

  state->music[0] = "crepuscolo_su_saturno";
  state->music[1] = "water";
  state->music[2] = "sangue_su_callisto";
  state->music[3] = "piano";
  state->music[4] = "fog_burning_dawn";

  if (hot_config.getString("game", "starting_screen") == "talking") {
    state->modes.push(new Talkin(state));
  } else if (hot_config.getString("game", "starting_screen") == "walking") {
    state->modes.push(new Walkin(state));
  }
  
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
