/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "walkin.h"

using namespace std;
using namespace Honey;

Walkin::Walkin(State* state) {
  this->state = state;
}

void Walkin::initialize() {
  player_direction = 1;
  player_x = hot_config.getInt("layout", "player_starting_x");
  player_y = hot_config.getInt("layout", "player_starting_y");
  player_margin_x = hot_config.getInt("layout", "player_margin_x");
  player_margin_y = hot_config.getInt("layout", "player_margin_y");
  player_vx = 0;
  player_vy = 0;

  state->camera_x = player_x - (state->screen_width / 2.0);
  state->camera_y = player_y - (state->screen_height / 1.5);

  hp_box = new Textbox(
    "Fonts/avenir_black.ttf",
    30,
    "TUNE BEAR " + to_string(state->player_hp) + "/1000",
    (position) {hot_config.getInt("layout", "hp_x"), hot_config.getInt("layout", "hp_y")},
    "#000000"
  );

  laps_box = new Textbox(
    "Fonts/avenir_black.ttf",
    30,
    "LAPS " + to_string(state->get("laps")),
    (position) {hot_config.getInt("layout", "laps_x"), hot_config.getInt("layout", "laps_y")},
    "#000000"
  );

  lap_zone_counter = 0;
  lap_zones = {
    {.top =  579, .left = 1520, .bottom = 622, .right = 1731},
    {.top =  222, .left = 869, .bottom = 429, .right = 974},
    {.top =  544, .left = 98, .bottom = 612, .right = 329},
    {.top =  739, .left = 1316, .bottom = 955, .right = 1409},
  };

  running_animation_speed = hot_config.getFloat("animation", "running_animation_speed");;
  frame = 1;
  timing.mark("running_animation");
}

void Walkin::movementLogic() {
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

  if (state->map->checkPath(player_x + player_vx, player_y + player_vy)) {
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
      if (!done && state->map->checkPath(player_x + p.x, player_y + p.y)) {
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
}

void Walkin::animationLogic() {
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
}

void Walkin::cameraLogic() {
  state->camera_target_x = player_x - (state->screen_width / 2.0);
  state->camera_target_y = player_y - (state->screen_height / 1.5);
  if (state->camera_x != state->camera_target_x) state->camera_x = state->camera_blend_factor * state->camera_x + (1 - state->camera_blend_factor) * state->camera_target_x;
  if (state->camera_y != state->camera_target_y) state->camera_y = state->camera_blend_factor * state->camera_y + (1 - state->camera_blend_factor) * state->camera_target_y;

  if (state->camera_x < 0) state->camera_x = 0;
  if (state->camera_x > state->map->map_width - state->screen_width) state->camera_x = state->map->map_width - state->screen_width;
  if (state->camera_y < 0) state->camera_y = 0;
  if (state->camera_y > state->map->map_height - state->screen_height) state->camera_y = state->map->map_height - state->screen_height;
}

void Walkin::gameLogic() {
  hp_box->setText("TUNE BEAR " + to_string(state->player_hp) + "/1000");
  laps_box->setText("LAPS " + to_string(state->get("laps")));

  if (input.keyPressed("r") > 0) {
    state->map->startRain();
  }
  if (input.keyPressed("s") > 0) {
    state->map->stopRain();
  }

  if (input.keyPressed("b") > 0) {
    state->modes.push(new Battlin(state));
  }

  for (int i = 0; i < lap_zones.size(); i++) {
    if (lap_zone_counter == i && checkBox(player_x, player_y, lap_zones[i])) {
      printf("Inside zone %d\n", i);
      lap_zone_counter += 1;
    }
    if (lap_zone_counter == 4) {
      lap_zone_counter = 0;
      state->store("laps", state->get("laps") + 1);
      printf("Lap.\n");
    }
  }
}

bool Walkin::checkBox(int x, int y, box b) {
  return (x >= b.left && x <= b.right && y >= b.top && y <= b.bottom);
}

void Walkin::logic() {
  movementLogic();
  animationLogic();
  cameraLogic();
  gameLogic();
}

void Walkin::render() {
  state->map->draw(-state->camera_x, -state->camera_y);

  string player_image = "Character_Test_Frame_" + to_string(frame) + ((player_direction == 1) ? "" : "_flip");
  graphics.drawImage(
    player_image,
    player_x - state->camera_x + player_margin_x,
    player_y - state->camera_y + player_margin_y,
    true,
    0,
    1
  );

  state->map->overlayer(-state->camera_x, -state->camera_y);

  hp_box->draw();
  laps_box->draw();
}

Walkin::~Walkin() {
  delete hp_box;
  delete laps_box;
}