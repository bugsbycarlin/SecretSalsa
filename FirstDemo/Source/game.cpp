/*
  BearsLoveHoney
  Matthew Carlin
  Copyright 2018
*/

#include "game.h"

using namespace std;
using namespace Honey;

Game::Game() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      extra_path[i][j] = {};
    }
  }
}

void Game::loop() {
  logic();
  render();
}

void Game::initialize() {
  screen_color = hot_config.getString("layout", "screen_color");
  screen_width = hot_config.getInt("layout", "screen_width");
  screen_height = hot_config.getInt("layout", "screen_height");
  map_name = hot_config.getString("layout", "map_name");

  graphics.addImages("Maps/", {
    map_name,
  });

  graphics.addImages("Art/", {
    "Character_Test_Frame_1",
    "Character_Test_Frame_2",
    "Character_Test_Frame_3",
    "Character_Test_Frame_1_flip",
    "Character_Test_Frame_2_flip",
    "Character_Test_Frame_3_flip",
  });

  map_width = graphics.getWidth(map_name);
  map_height = graphics.getHeight(map_name);

  loadPath();

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

void Game::loadPath() {
  std::ifstream input_file("Scripts/" + map_name + "_path.txt");

  string line;
  int count = 0;
  while (getline(input_file, line)) {
    if (count > 0) {
      std::vector<std::string> words;
      boost::split(words, line, boost::is_any_of(","), boost::token_compress_on);
      int x = stoi(words[0]);
      int y = stoi(words[1]);
      int r = stoi(words[2]);
      if (r != 0) {
        path.push_back({x,y,r});
      } else {
        int zone_x = x / (map_width / 8.0);
        int zone_y = y / (map_height / 8.0);
        extra_path[zone_x][zone_y].push_back({x,y,r});
      }
    }
    count++;
  }
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

  if (checkPath(player_x + player_vx, player_y + player_vy)) {
    player_x += player_vx;
    player_y += player_vy;
  } else {
    point p = math_utils.rotateVector(player_vx, player_vy, velocity_tolerance);
    if (checkPath(player_x + p.x, player_y + p.y)) {
      player_x += p.x;
      player_y += p.y;
      printf("CounterClock success\n");
      printf("V orig: %0.3f, %0.3f\n", player_vx, player_vy);
      printf("V rota: %0.3f, %0.3f\n", p.x, p.y);
      p = math_utils.rotateVector(player_vx, player_vy, 0.2 * velocity_tolerance);
      player_vx = restitution * p.x;
      player_vy = restitution * p.y;
    } else {
      p = math_utils.rotateVector(player_vx, player_vy, -velocity_tolerance);
      if (checkPath(player_x + p.x, player_y + p.y)) {
        player_x += p.x;
        player_y += p.y;
        printf("Clock success\n");
        printf("V orig: %0.3f, %0.3f\n", player_vx, player_vy);
        printf("V rota: %0.3f, %0.3f\n", p.x, p.y);
        p = math_utils.rotateVector(player_vx, player_vy, -0.2 * velocity_tolerance);
        player_vx = restitution * p.x;
        player_vy = restitution * p.y;
      }
    }
  }
  

  // else if (checkPath(player_x + 0.5 * player_vx, player_y + player_vy + bump_margin)) {
  //   player_x = player_x + 0.5 * player_vx;
  //   player_y = player_y + player_vy + bump_margin;
  // } else if (checkPath(player_x + player_vx + bump_margin, player_y + 0.5 * player_vy)) {
  //   player_x = player_x + player_vx + bump_margin;
  //   player_y = player_y + 0.5 * player_vy;
  // } else if (checkPath(player_x + 0.5 * player_vx, player_y + player_vy - bump_margin)) {
  //   player_x = player_x + 0.5 * player_vx;
  //   player_y = player_y + player_vy - bump_margin;
  // } else if (checkPath(player_x + player_vx - bump_margin, player_y + 0.5 * player_vy)) {
  //   player_x = player_x + player_vx - bump_margin;
  //   player_y = player_y + 0.5 * player_vy;
  // }
  
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
  if (camera_x > map_width - screen_width) camera_x = map_width - screen_width;
  if (camera_y < 0) camera_y = 0;
  if (camera_y > map_height - screen_height) camera_y = map_height - screen_height;

  if (input.keyPressed("quit") > 0) {
    screenmanager.setQuit();
  }

  if (input.threeQuickKey("escape")) {
    screenmanager.setQuit();
  }
}

bool Game::checkPath(int x, int y) {
  for (map_circle circle : path) {
    if (math_utils.distance(x, y, circle.x, circle.y) <= circle.r) {
      return true;
    }
  }
  int zone_x = x / (map_width / 8.0);
  int zone_y = y / (map_height / 8.0);
  for (map_circle circle : extra_path[zone_x][zone_y]) {
    if (x == circle.x && y == circle.y) {
      return true;
    }
  }
  return false;
}

void Game::render() {
  // Clear the screen to a soft white color
  graphics.clearScreen(screen_color);

  // Switch to 2D drawing mode
  graphics.draw2D();

  graphics.drawImage(map_name, -camera_x, -camera_y);

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
}
