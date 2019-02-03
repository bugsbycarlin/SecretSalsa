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

  timing.mark("battlin_hp_loss");
}

void Battlin::logic() {
  hp_box->setText("TUNE BEAR " + to_string(state->player_hp) + "/1000");

  if (timing.since("battlin_hp_loss") > 0.5) {
    state->player_hp -= 10;
    timing.mark("battlin_hp_loss");
  }

  if (input.keyPressed("b") > 0) {
    for (Placement placement : left_placements) {
      placement.character->hp = 0;
    }
    // TO DO: delete this tired old mode
    state->modes.pop();
  }
}

void Battlin::render() {
  state->map->draw(-state->camera_x, -state->camera_y);

  // for (int i = 0; i < 2; i++) {
  //   for (int j = 0; j < 4; j++) {
  //     position p = layouts.tile("battle_left_layout", i, j);
  //     position p2 = layouts.tile("battle_right_layout", i, j);
  //     if (i % 3 == 0 || j == i) {
  //       graphics.drawImage(
  //         "baddie_static",
  //         // hot_config.getInt("layout", "player_battlin_x"),
  //         // hot_config.getInt("layout", "player_battlin_y"),
  //         p.x + hot_config.getInt("layout", "player_margin_x") - hot_config.getInt("layout", "battle_layout_tilt") * j,
  //         p.y + hot_config.getInt("layout", "player_margin_y"),
  //         true,
  //         0,
  //         1, true, false, false
  //       );

  //       graphics.drawImage(
  //         "tune_bear",
  //         // hot_config.getInt("layout", "player_battlin_x"),
  //         // hot_config.getInt("layout", "player_battlin_y"),
  //         p2.x + hot_config.getInt("layout", "player_margin_x") + hot_config.getInt("layout", "battle_layout_tilt") * j,
  //         p2.y + hot_config.getInt("layout", "player_margin_y"),
  //         true,
  //         0,
  //         1, false, false, false
  //       );
  //     }
  //   }
  // }

  for (Placement placement : left_placements) {
    position p = layouts.tile("battle_left_layout", placement.i, placement.j);
    graphics.drawImage(
      placement.character->animations["static"][0],
      p.x + hot_config.getInt("layout", "player_margin_x") - hot_config.getInt("layout", "battle_layout_tilt") * placement.j,
      p.y + hot_config.getInt("layout", "player_margin_y"),
      true,
      0,
      1, true, false, false
    );
  }

  for (Placement placement : right_placements) {
    position p = layouts.tile("battle_right_layout", placement.i, placement.j);
    graphics.drawImage(
      placement.character->animations["static"][0],
      p.x + hot_config.getInt("layout", "player_margin_x") + hot_config.getInt("layout", "battle_layout_tilt") * placement.j,
      p.y + hot_config.getInt("layout", "player_margin_y"),
      true,
      0,
      1, false, false, false
    );
  }

  state->map->overlayer(-state->camera_x, -state->camera_y);

  hp_box->draw();
}

Battlin::~Battlin() {
}