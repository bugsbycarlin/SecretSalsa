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
  //initialize();
}

void Battlin::initialize() {
  mode = "prep";

  ap_delay = hot_config.getInt("game", "ap_delay");

  // hp_box = new Textbox(
  //   "Fonts/avenir_black.ttf",
  //   30,
  //   "TUNE BEAR " + to_string(state->player_hp) + "/1000",
  //   (position) {hot_config.getInt("layout", "hp_x"), hot_config.getInt("layout", "hp_y")},
  //   "#000000"
  // );

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

  for (Placement placement : left_placements) {
    position p = layouts.tile("battle_left_layout", placement.i, placement.j);
    placement.character->battle_x = placement.character->x - state->camera_x;
    placement.character->battle_y = placement.character->y - state->camera_y;
    placement.character->battle_home_x = p.x - hot_config.getInt("layout", "battle_layout_tilt") * placement.j;
    placement.character->battle_home_y = p.y;
    printf("%s got placed at %d, %d\n", placement.character->animations["static"][0].c_str(), placement.character->battle_x, placement.character->battle_y);
    // graphics.drawImage(
    //   placement.character->animations["static"][0],
    //   p.x + hot_config.getInt("layout", "player_margin_x") - hot_config.getInt("layout", "battle_layout_tilt") * placement.j,
    //   p.y + hot_config.getInt("layout", "player_margin_y"),
    //   true,
    //   0,
    //   1, true, false, false
    // );
  }

  for (Placement placement : right_placements) {
    position p = layouts.tile("battle_right_layout", placement.i, placement.j);
    placement.character->battle_x = placement.character->x - state->camera_x;
    placement.character->battle_y = placement.character->y - state->camera_y;
    placement.character->battle_home_x = p.x + hot_config.getInt("layout", "battle_layout_tilt") * placement.j;
    placement.character->battle_home_y = p.y;
    printf("%s got placed at %d, %d\n", placement.character->animations["static"][0].c_str(), placement.character->battle_x, placement.character->battle_y);
    // graphics.drawImage(
    //   placement.character->animations["static"][0],
    //   p.x + hot_config.getInt("layout", "player_margin_x") + hot_config.getInt("layout", "battle_layout_tilt") * placement.j,
    //   p.y + hot_config.getInt("layout", "player_margin_y"),
    //   true,
    //   0,
    //   1, false, false, false
    // );
  }

  int character_count = 0;
  for (Character* character : party) {
    printf("Party character name is %s\n", character->name.c_str());
    names[character->name] = new Menu(
      hot_config.getInt("menu", "battle_name_x") + 300 * character_count,
      hot_config.getInt("menu", "battle_name_y"),
      hot_config.getInt("menu", "battle_name_width"),
      hot_config.getInt("menu", "battle_name_height"),
      "Art/",
      hot_config.getString("menu", "battle_name_image_root"),
      hot_config.getInt("menu", "battle_name_margin_x"),
      hot_config.getInt("menu", "battle_name_margin_y"),
      hot_config.getInt("menu", "battle_name_num_lines"),
      hot_config.getInt("menu", "battle_name_wrap_length"),
      hot_config.getBool("menu", "battle_name_typewriter"),
      hot_config.getFloat("menu", "battle_name_typewriter_delay"),
      hot_config.getString("menu", "battle_name_font_path"),
      hot_config.getInt("menu", "battle_name_font_size"),
      hot_config.getString("menu", "battle_name_font_color")
    );
    names[character->name]->setText(character->display_name);

    info_cards[character->name] = new Menu(
      hot_config.getInt("menu", "battle_info_card_x") + 300 * character_count,
      hot_config.getInt("menu", "battle_info_card_y"),
      hot_config.getInt("menu", "battle_info_card_width"),
      hot_config.getInt("menu", "battle_info_card_height"),
      "Art/",
      hot_config.getString("menu", "battle_info_card_image_root"),
      hot_config.getInt("menu", "battle_info_card_margin_x"),
      hot_config.getInt("menu", "battle_info_card_margin_y"),
      hot_config.getInt("menu", "battle_info_card_num_lines"),
      hot_config.getInt("menu", "battle_info_card_wrap_length"),
      hot_config.getBool("menu", "battle_info_card_typewriter"),
      hot_config.getFloat("menu", "battle_info_card_typewriter_delay"),
      hot_config.getString("menu", "battle_info_card_font_path"),
      hot_config.getInt("menu", "battle_info_card_font_size"),
      hot_config.getString("menu", "battle_info_card_font_color")
    );

    character_count++;
  }
}

void Battlin::logic() {
  if (mode == "prep") {
    bool ready = true;
    for (Placement placement : left_placements) {
      if (placement.character->battle_home_x != placement.character->battle_x
        || placement.character->battle_home_y != placement.character->battle_y) {
        ready = false;
        placement.character->battlePrepBehavior();
      }
    }

    for (Placement placement : right_placements) {
      if (placement.character->battle_home_x != placement.character->battle_x
        || placement.character->battle_home_y != placement.character->battle_y) {
        ready = false;
        placement.character->battlePrepBehavior();
      }
    }

    if (ready) {
      for (Placement placement : left_placements) {
        placement.character->ap = 0;
      }

      for (Placement placement : right_placements) {
        placement.character->ap = 0;
      }
      mode = "active";
    }
  }

  if (mode == "active") {
    //hp_box->setText("TUNE BEAR " + to_string(state->player_hp) + "/1000");

    // if (timing.since("battlin_hp_loss") > 0.5) {
    //   state->player_hp -= 10;
    //   timing.mark("battlin_hp_loss");
    // }

    if (!timing.check("ap_delay")) {
      timing.mark("ap_delay");
    } else if (timing.since("ap_delay") > ap_delay) {
      for (Placement placement : left_placements) {
        placement.character->ap += placement.character->ap_rate;
        if (placement.character->ap > placement.character->max_ap) {
          placement.character->ap = placement.character->max_ap;
        }
      }

      for (Placement placement : right_placements) {
        placement.character->ap += placement.character->ap_rate;
        if (placement.character->ap > placement.character->max_ap) {
          placement.character->ap = placement.character->max_ap;
        }
      }
      timing.mark("ap_delay");
    }

    if (input.keyPressed("b") > 0) {
      for (Placement placement : left_placements) {
        if (placement.character->animations["static"][0] == "baddie_static") {
          placement.character->hp = 0;
        }
      }
      for (Placement placement : right_placements) {
        if (placement.character->animations["static"][0] == "baddie_static") {
          placement.character->hp = 0;
        }
      }
      // TO DO: delete this tired old mode
      state->modes.pop();
    }
  }

  for (Character* character : party) {
    info_cards[character->name]->setTextLines({
      "HP: " + to_string(character->hp) + " / " + to_string(character->max_hp),
      "AP: " + to_string((int) character->ap) + " / " + to_string((int) character->max_ap)
    });
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



  // for (Placement placement : left_placements) {
  //   position p = layouts.tile("battle_left_layout", placement.i, placement.j);
  //   graphics.drawImage(
  //     placement.character->animations["static"][0],
  //     p.x + hot_config.getInt("layout", "player_margin_x") - hot_config.getInt("layout", "battle_layout_tilt") * placement.j,
  //     p.y + hot_config.getInt("layout", "player_margin_y"),
  //     true,
  //     0,
  //     1, true, false, false
  //   );
  // }

  // for (Placement placement : right_placements) {
  //   position p = layouts.tile("battle_right_layout", placement.i, placement.j);
  //   graphics.drawImage(
  //     placement.character->animations["static"][0],
  //     p.x + hot_config.getInt("layout", "player_margin_x") + hot_config.getInt("layout", "battle_layout_tilt") * placement.j,
  //     p.y + hot_config.getInt("layout", "player_margin_y"),
  //     true,
  //     0,
  //     1, false, false, false
  //   );
  // }

  if (mode == "prep") {
    for (Placement placement : left_placements) {
      placement.character->battlePrepDraw();
    }

    for (Placement placement : right_placements) {
      placement.character->battlePrepDraw();
    }
  }

  if (mode == "active") {
    for (Placement placement : left_placements) {
      placement.character->battleDraw();
    }

    for (Placement placement : right_placements) {
      placement.character->battleDraw();
    }

    for (Character* character : party) {
      names[character->name]->draw();
      info_cards[character->name]->draw();
    }
  }

  state->map->overlayer(-state->camera_x, -state->camera_y);

  //hp_box->draw();
}

Battlin::~Battlin() {
}