/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "honey.h"

#include <string>
#include <array>

using namespace Honey;
using namespace std;

position player_position;

int step_width;
float frame_delay;

int current_frame;

bool quit = false;

void initialize() {
  player_position = {
    hot_config.getInt("walk", "starting_x"),
    hot_config.getInt("walk", "starting_y")
  };

  step_width = hot_config.getInt("walk", "step_width");
  frame_delay = hot_config.getFloat("walk", "frame_delay");

  graphics.addImages("Art/", {
    "Walk1",
    "Walk2",
    "Walk3",
    "Walk4",
  });

  current_frame = 4;
}

void logic() {
  if (input.keyPressed("quit") > 0) {
    quit = true;
  }

  if (input.threeQuickKey("escape")) {
    quit = true;
  }

  if (!timing.check("animate") || timing.since("animate") > frame_delay) {
    current_frame += 1;
    if (current_frame > 4) current_frame = 1;
    if (current_frame == 2 || current_frame == 4) {
      player_position.x += 85;
    } else {
      player_position.x += 5;
    }
    timing.mark("animate");
  }

  if (player_position.x > hot_config.getInt("layout", "screen_width")) {
    player_position.x = -250;
  }
}

void render() {
  graphics.clearScreen("#EEEEEE");
  graphics.setColor("#FFFFFF", 1.0);

  graphics.drawImage(
    "Walk" + to_string(current_frame),
    player_position.x,
    player_position.y
  );
}

int main(int argc, char* args[]) {
  StartHoney("Secret Salsa WorldBuilder");

  graphics.draw2D();

  initialize();

  while (!quit) {
    input.processInput();

    logic();
    render();

    graphics.updateDisplay();
  }
}
