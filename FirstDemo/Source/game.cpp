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
  modes = {};
  modes.push(new Walkin());
}

void Game::loop() {
  logic();
  modes.top()->logic();
  render();
  modes.top()->render();
}

void Game::initialize() {
  screen_color = hot_config.getString("layout", "screen_color");
  screen_width = hot_config.getInt("layout", "screen_width");
  screen_height = hot_config.getInt("layout", "screen_height");

  map = new Map(hot_config.getString("layout", "map_name"));

  graphics.addImages("Art/", {
    "Character_Test_Frame_1",
    "Character_Test_Frame_2",
    "Character_Test_Frame_3",
    "Character_Test_Frame_1_flip",
    "Character_Test_Frame_2_flip",
    "Character_Test_Frame_3_flip",
  });

  player_direction = 1;
  player_x = hot_config.getInt("layout", "player_starting_x");
  player_y = hot_config.getInt("layout", "player_starting_y");
  player_margin_x = hot_config.getInt("layout", "player_margin_x");
  player_margin_y = hot_config.getInt("layout", "player_margin_y");
  player_vx = 0;
  player_vy = 0;

  camera_x = 0;
  camera_y = 0;
  camera_target_x = player_x;
  camera_target_y = player_y;

  running_animation_speed = hot_config.getFloat("animation", "running_animation_speed");;
  frame = 1;
  timing.mark("running_animation");
}

void Game::logic() {
  
  if (input.keyDown("left")) {
    player_vx -= player_ax;
  }
  if (input.keyDown("right")) {
    player_vx += player_ax;
  }
  if (input.keyDown("up")) {
    player_vy -= player_ay;
  }
  if (input.keyDown("down")) {
    player_vy += player_ay;
  }

  if (player_vx > player_max_velocity) player_vx = player_max_velocity;
  if (player_vx < -player_max_velocity) player_vx = -player_max_velocity;
  if (player_vy > player_max_velocity) player_vy = player_max_velocity;
  if (player_vy < -player_max_velocity) player_vy = -player_max_velocity;

  if (map->checkPath(player_x + player_vx, player_y + player_vy)) {
    player_x += player_vx;
    player_y += player_vy;
  } else {
    bool done = false;
    vector<float> angles = {
      velocity_tolerance / 10.0f,
      -velocity_tolerance / 10.0f,
      velocity_tolerance / 3.0f,
      -velocity_tolerance / 3.0f,
      velocity_tolerance / 1.5f,
      -velocity_tolerance / 1.5f,
      velocity_tolerance,
      -velocity_tolerance
    };
    for (float angle : angles) {
      point p = math_utils.rotateVector(player_vx, player_vy, angle);
      if (!done && map->checkPath(player_x + p.x, player_y + p.y)) {
        player_x += p.x;
        player_y += p.y;
        player_vx = restitution * p.x;
        player_vy = restitution * p.y;
        done = true;
      }
    }
  }
  
  player_vx *= player_velocity_decay;
  player_vy *= player_velocity_decay;
  if (abs(player_vx) > 0.1) {
    if (player_vx >= 0) {
      player_direction = 1;
    } else {
      player_direction = -1;
    }
  }

  float tv = math_utils.distance(0, 0, player_vx, player_vy);
  if (tv > 1.0) {
    if (timing.since("running_animation") > 1.0 / (tv * running_animation_speed)) {
      if (frame == 2) {
        frame = 3;
      } else {
        frame = 2;
      }
      timing.mark("running_animation");
    }
  } else {
    frame = 1;
    timing.mark("running_animation");
  }

  if (camera_x != player_x) camera_x = camera_blend_factor * camera_x + (1 - camera_blend_factor) * (player_x - camera_target_x);
  if (camera_y != player_y) camera_y = camera_blend_factor * camera_y + (1 - camera_blend_factor) * (player_y - camera_target_y);

  if (camera_x < 0) camera_x = 0;
  if (camera_x > map->map_width - screen_width) camera_x = map->map_width - screen_width;
  if (camera_y < 0) camera_y = 0;
  if (camera_y > map->map_height - screen_height) camera_y = map->map_height - screen_height;

  if (input.keyPressed("quit") > 0) {
    screenmanager.setQuit();
  }

  if (input.threeQuickKey("escape")) {
    screenmanager.setQuit();
  }
}

void Game::render() {
  // Clear the screen to a soft white color
  graphics.clearScreen(screen_color);

  // Switch to 2D drawing mode
  graphics.draw2D();

  map->draw(-camera_x, -camera_y);

  string player_image = "Character_Test_Frame_" + to_string(frame) + ((player_direction == 1) ? "" : "_flip");
  graphics.drawImage(
    player_image,
    player_x - camera_x + player_margin_x,
    player_y - camera_y + player_margin_y,
    true,
    0,
    1
  );
}

Game::~Game() {
  delete map;
}
