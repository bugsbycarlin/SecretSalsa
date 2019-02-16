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
  printf("Here 0\n");
  state->screen_color = hot_config.getString("layout", "screen_color");
  state->screen_width = hot_config.getInt("layout", "screen_width");
  state->screen_height = hot_config.getInt("layout", "screen_height");

  state->map = new Map(hot_config.getString("layout", "map_name"));

  state->store("laps", 0);

  input.addActionKey("up", "up");
  input.addActionKey("down", "down");
  input.addActionKey("left", "left");
  input.addActionKey("right", "right");
  input.addActionKey("accept", "a");
  input.addActionKey("cancel", "s");

  graphics.addImages("Art/", {
    "intro_screen",
    "bigdog_static",
    "bigdog_walk_2_3",
    "bigdog_walk_2_4",
    "bigdog_hurt",
    "bigdog_ko",
    "bigdog_kick",
    "coach_bulldog_profile",
    "coach_bulldog",
    "tune_bear_profile",
    "tune_bear",
    "tune_bear_hurt",
    "tune_bear_ko",
    "robin",
    "robin_hurt",
    "robin_ko",
    "robin_flapping",
    "witchycat",
    "witchycat_speak",
    "witchycat_hurt",
    "witchycat_ko",
    "choice_arrow",
    "choice_arrow_2",
    "choice_hand",
    "charge_gauge_outline",
    "charge_gauge_orange_fill",
    "charge_gauge_green_fill",
    "attack_slash_thick",
    "black_screen",
    "glasses",
    "spellbook",
    "Coffee",
    "Honey",
    "Chicken"
  });

  printf("Here 1\n");
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

  printf("Here 2\n");
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
    "deep_rough",
    "sangue_su_callisto",
    "sunset_cove",
    "fog_burning_dawn",
    "battle_fanfare",
    "game_over",
    "win_music"
  });

  sound.addSounds("Sound/", {
    "select_sound",
    "accept_sound",
    "cancel_sound",
    "bigdog_attack",
    "tune_bear_attack",
    "robin_attack",
    "witchycat_attack",
    "miss",
    "thunder",
    "ko_1",
    "ko_2",
    "coach_bulldog_speech_1",
    "coach_bulldog_speech_2",
    "tune_bear_speech_1",
    "tune_bear_speech_2",
    "ice_shatter_1",
    "ice_shatter_2",
    "ice_shatter_3",
    "ice_shatter_4",
    "ice_shatter_5",
    "toot",
    "dust",
  });

  state->music["music_1"] = "crepuscolo_su_saturno";
  state->music["music_2"] = "water";
  state->music["music_3"] = "sangue_su_callisto";
  state->music["music_4"] = "piano";
  state->music["music_5"] = "fog_burning_dawn";

  state->store("switch_music_for_battle", 1);

  sound.setMusicVolume(hot_config.getFloat("music", "music_volume"));
  sound.setSoundVolume(hot_config.getFloat("music", "sound_volume"));

  if (hot_config.getString("game", "starting_screen") == "talking") {
    state->modes.push(new Talkin(state));
  } else if (hot_config.getString("game", "starting_screen") == "walking") {
    state->modes.push(new Walkin(state));
  } else if (hot_config.getString("game", "starting_screen") == "intro") {
    state->modes.push(new Intro(state));
  }
  
  printf("Here 3\n");
  state->modes.top()->initialize();
  printf("Here 4\n");
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
