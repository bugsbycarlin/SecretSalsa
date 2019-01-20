/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "battlin.h"

using namespace std;
using namespace Honey;

Battlin::Battlin(State* state) {
  this->state = state;
  initialize();
}

void Battlin::initialize() {
  hp_box = new Textbox(
    "Fonts/avenir_black.ttf",
    30,
    "TUNE BEAR " + to_string(state->player_hp) + "/1000",
    (position) {hot_config.getInt("layout", "hp_x"), hot_config.getInt("layout", "hp_y")},
    "#000000"
  );

  timing.mark("battlin_hp_loss");
}

void Battlin::logic() {
  hp_box->setText("TUNE BEAR " + to_string(state->player_hp) + "/1000");

  if (timing.since("battlin_hp_loss") > 0.5) {
    state->player_hp -= 10;
    timing.mark("battlin_hp_loss");
  }

  if (input.keyPressed("b") > 0) {
    // TO DO: delete this tired old mode
    state->modes.pop();
  }
}

void Battlin::render() {
  state->map->draw(-state->camera_x, -state->camera_y);

  graphics.drawImage(
    "Character_Test_Frame_1_flip",
    hot_config.getInt("layout", "player_battlin_x"),
    hot_config.getInt("layout", "player_battlin_y"),
    true,
    0,
    1
  );

  hp_box->draw();
}

Battlin::~Battlin() {
}