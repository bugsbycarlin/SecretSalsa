/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "battlin.h"

using namespace std;
using namespace Honey;


Battlin::Battlin(State* state, vector<WalkingCharacter*> player_party, vector<WalkingCharacter*> enemy_party) {
  this->state = state;
  good_direction = 1;
  root_player_party = player_party;
  root_enemy_party = enemy_party;
}

void Battlin::initialize() {
  mode = "prep";

  ap_delay = hot_config.getFloat("game", "ap_delay");

  initializeCharacters();
  initializeMenus();

  damage_text = new Textbox(
    "Fonts/IBMPlexMono-Bold.ttf",
    30,
    " ",
    (position) {0, 0},
    "#bb0000"
  );

  black_screen = new Sprite(
    "black_screen",
    {640, 360},
    "#ffffff",
    0.0, 0.0, 1.0
  );

  selection_character = NULL;
  acting_enemy = NULL;
  timing.mark("done_selecting");
}

void Battlin::initializeCharacters() {
  int good_col = 1;
  int bad_col = 0;
  string good_place = "left";
  string bad_place = "right";
  if (good_direction == -1) {
    good_col = 0;
    bad_col = 1;
    good_place = "right";
    bad_place = "left";
  }  
  int j = 0;
  for (Character* b : root_enemy_party) {
    BattleCharacter* bc = new BattleCharacter(state);
    bc->cloneFromCharacter(b);
    bc->addBounceAnimation();
    bc->battle_row = j;
    bc->battle_column = bad_col;
    bc->battle_direction = -1 * good_direction;
    bc->player_character = false;

    bc->attack_jump_time = hot_config.getFloat("game", "bigdog_battle_jump_time");
    bc->attack_hold_time = hot_config.getFloat("game", "bigdog_battle_hold_time");

    position p = layouts.tile("battle_" + bad_place + "_layout", bc->battle_column, bc->battle_row);
    bc->battle_x = b->x - state->camera_x;
    bc->battle_y = b->y - state->camera_y;
    bc->battle_home_x = p.x + good_direction * hot_config.getInt("layout", "battle_layout_tilt") * bc->battle_row;
    bc->battle_home_y = p.y;

    enemy_party.push_back(bc);
    everyone.push_back(bc);
    j += 1;
  }
  j = 0;
  for (Character* g : root_player_party) {
    BattleCharacter* bc = new BattleCharacter(state);
    bc->cloneFromCharacter(g);
    bc->addBounceAnimation();
    bc->battle_row = j;
    bc->battle_column = good_col;
    bc->battle_direction = good_direction;
    bc->player_character = true;

    position p = layouts.tile("battle_" + good_place + "_layout", bc->battle_column, bc->battle_row);
    bc->battle_x = g->x - state->camera_x;
    bc->battle_y = g->y - state->camera_y;
    bc->battle_home_x = p.x + -1 * good_direction * hot_config.getInt("layout", "battle_layout_tilt") * bc->battle_row;
    bc->battle_home_y = p.y;

    player_party.push_back(bc);
    everyone.push_back(bc);
    j += 1;
  }
}

void Battlin::initializeMenus() {
  header = new Menu(
    //+ (good_direction == -1 ? 640 : 0)
    hot_config.getInt("menu", "battle_header_x"),
    hot_config.getInt("menu", "battle_header_y"),
    hot_config.getInt("menu", "battle_header_width"),
    hot_config.getInt("menu", "battle_header_height"),
    "Art/",
    hot_config.getString("menu", "battle_header_image_root"),
    hot_config.getInt("menu", "battle_header_margin_x"),
    hot_config.getInt("menu", "battle_header_margin_y"),
    hot_config.getInt("menu", "battle_header_num_lines"),
    hot_config.getInt("menu", "battle_header_wrap_length"),
    hot_config.getBool("menu", "battle_header_typewriter"),
    hot_config.getFloat("menu", "battle_header_typewriter_delay"),
    hot_config.getString("menu", "battle_header_font_path"),
    hot_config.getInt("menu", "battle_header_font_size"),
    hot_config.getString("menu", "battle_header_font_color")
  );
  string s = printLine({"Name", "HP", "SP", "Charge"}, {11, 8, 6, 20});
  printf("Here: %s---\n", s.c_str());
  header->setTextLines({s});

  info_card = new Menu(
    hot_config.getInt("menu", "battle_info_card_x"),
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

  selection_card_1_header = new Menu(
    hot_config.getInt("menu", "selection_card_1_header_x"),
    hot_config.getInt("menu", "selection_card_1_header_y"),
    hot_config.getInt("menu", "selection_card_1_header_width"),
    hot_config.getInt("menu", "selection_card_1_header_height"),
    "Art/",
    hot_config.getString("menu", "selection_card_1_header_image_root"),
    hot_config.getInt("menu", "selection_card_1_header_margin_x"),
    hot_config.getInt("menu", "selection_card_1_header_margin_y"),
    hot_config.getInt("menu", "selection_card_1_header_num_lines"),
    hot_config.getInt("menu", "selection_card_1_header_wrap_length"),
    hot_config.getBool("menu", "selection_card_1_header_typewriter"),
    hot_config.getFloat("menu", "selection_card_1_header_typewriter_delay"),
    hot_config.getString("menu", "selection_card_1_header_font_path"),
    hot_config.getInt("menu", "selection_card_1_header_font_size"),
    hot_config.getString("menu", "selection_card_1_header_font_color")
  );
  selection_card_1_header->setTextLines({"Action"});

  selection_card_1 = new Menu(
    hot_config.getInt("menu", "selection_card_1_x"),
    hot_config.getInt("menu", "selection_card_1_y"),
    hot_config.getInt("menu", "selection_card_1_width"),
    hot_config.getInt("menu", "selection_card_1_height"),
    "Art/",
    hot_config.getString("menu", "selection_card_1_image_root"),
    hot_config.getInt("menu", "selection_card_1_margin_x"),
    hot_config.getInt("menu", "selection_card_1_margin_y"),
    hot_config.getInt("menu", "selection_card_1_num_lines"),
    hot_config.getInt("menu", "selection_card_1_wrap_length"),
    hot_config.getBool("menu", "selection_card_1_typewriter"),
    hot_config.getFloat("menu", "selection_card_1_typewriter_delay"),
    hot_config.getString("menu", "selection_card_1_font_path"),
    hot_config.getInt("menu", "selection_card_1_font_size"),
    hot_config.getString("menu", "selection_card_1_font_color")
  );
  selection_card_1->setTextLines({"Attack", "Special", "Move", "Item"});
}

void Battlin::logic() {
  //printf("Mode is %s\n", mode.c_str());

  if (mode == "prep") {
    if (!timing.check("music_down")) timing.mark("music_down");
    if (timing.since("music_down") < 1) {
      sound.setMusicVolume(1.0 - timing.since("music_down"));
    }

    everybodyGetInPlace(); // mode might change to charging

    if (mode == "charging") { // it was just switched
      sound.stopMusic();
      sound.playMusic("battle_fanfare", -1);
      sound.setMusicVolume(1.0);
    }
  }

  if (mode == "acting") {
    // For now
    selection_character->continueAttack();
    if (selection_character->action_state == "waiting") {
      selection_character = NULL;
      selection_1 = 0;
      selection_2 = 0;
      selection_row = 0;
      selection_column = 0;
      selection_state = 1;
      mode = "charging";
    }
  }

  if (mode == "charging" || mode == "selecting" || mode == "acting") {
    chargeGauges();
    
    // if (input.keyPressed("b") > 0) {
    //   for (Character* character : root_enemy_party) {
    //     character->hp = 0;
    //   }

    //   // TO DO: delete this tired old mode
    //   state->modes.pop();
    // }
  }

  if (mode == "charging" && acting_enemy == NULL) {
    if (action_queue.size() > 0 && timing.since("done_selecting") > 0.2) {
      BattleCharacter* character = action_queue.front();
      action_queue.pop();
      if (character->hp > 0) {
        if (!character->player_character) {
          if (character->hp > 0) {
            // Do enemy behavior
            enemy_queue.push(character);
          }
        } else {
          selection_character = character;
          selection_character->action_state = "choosing";
          mode = "selecting";
          selection_1 = 0;
          selection_2 = 0;
          selection_row = 0;
          selection_column = 0;
          selection_state = 1;
        }
      }
    }

    if (enemy_queue.size() > 0 && acting_enemy == NULL && mode != "selecting") {
      acting_enemy = enemy_queue.front();
      enemy_queue.pop();
      if (acting_enemy-> hp > 0) {
        acting_enemy->startAutomaticBattle(player_party);
      } else {
        acting_enemy = NULL;
      }
    }
  }

  if (acting_enemy != NULL)  {
    acting_enemy->continueAttack();
  }

  if (acting_enemy != NULL && acting_enemy->action_state == "waiting") {
    acting_enemy = NULL;
  }

  if (mode == "selecting") {
    handleSelection();
  }

  bool enemy_defeated = true;
  for (BattleCharacter* character : enemy_party) {
    if (character->hp > 0) enemy_defeated = false;
  }
  bool player_defeated = true;
  for (BattleCharacter* character : player_party) {
    if (character->hp > 0) player_defeated = false;
  }

  if (mode != "finished" && (player_defeated || enemy_defeated)) {
    mode = "finished";
    timing.mark("finished_battlin");
    black_screen->setOpacity(1.0, hot_config.getFloat("game", "battle_finish_time"));
    timing.mark("music_down");
  }

  if (mode == "finished" && timing.check("music_down")) {
    if (timing.since("music_down") < 1) {
      sound.setMusicVolume(1.0 - timing.since("music_down"));
    }
  }

  if (mode == "finished" && timing.since("finished_battlin") > hot_config.getFloat("game", "battle_finish_time")) {
    if (!player_defeated) {
      // Move HP into permanent characters.
      for (BattleCharacter* character : player_party) {
        character->permanent_character->hp = character->hp;
      }
      // Get out of battle
      state->modes.pop();
    }
  }
}

void Battlin::everybodyGetInPlace() {
  bool ready = true;
    for (BattleCharacter* character : everyone) {
      if (character->battle_home_x != character->battle_x
        || character->battle_home_y != character->battle_y) {
        ready = false;
        character->walkToStartingPosition();
      }
    }

    if (ready) {
      for (BattleCharacter* character : everyone) {
        character->ap = 0;
      }
      mode = "charging";
    }
}

void Battlin::chargeGauges() {
  if (!timing.check("ap_delay")) {
      timing.mark("ap_delay");
    } else if (timing.since("ap_delay") > ap_delay) {
      for (BattleCharacter* character : everyone) {
        if (character->action_state == "waiting" && character->hp > 0) {
          character->ap += character->ap_rate;

          // If a character is facing the wrong way, when the gauge is charged, turn around and reset the gauge.
          if (character->ap >= character->max_ap && character->battle_direction != character->direction) {
            character->direction = character->battle_direction;
            character->ap = 0;
          }

          if (character->ap >= character->max_ap) {
            character->ap = character->max_ap;
            character->action_state = "ready";
            action_queue.push(character);
          }
        }
      }
      timing.mark("ap_delay");
    }
}

void Battlin::handleSelection() {
  if (selection_state == 1) {
    if (input.keyPressed("down") > 0) {
      sound.playSound("select_sound", 1);
      selection_1 += 1;
      if (selection_1 > selection_1_max) {
        selection_1 = 0;
      }
    } else if (input.keyPressed("up") > 0) {
      sound.playSound("select_sound", 1);
      selection_1 -= 1;
      if (selection_1 < 0) {
        selection_1 = selection_1_max;
      }
    } else if (input.keyPressed("a") > 0) {
      sound.playSound("accept_sound", 1);
      // choose the choice!
      if (selection_1 == 0) {
        //mode = "acting";
        //selection_character->action_state = "acting";
        selection_state = 2;
        for (int i = 0; i < enemy_party.size() - 1; i++) {
          if (enemy_party[selection_2]-> hp <= 0) {
            selection_2 += 1;
          }
        }
      } else {
        mode = "charging";
        selection_character->ap = 0;
        selection_character->action_state = "waiting";
        timing.mark("done_selecting");
      }
    }
  } else if (selection_state == 2) {
    if (input.keyPressed("down") > 0) {
      sound.playSound("select_sound", 1);
      selection_2 += 1;
      if (selection_2 > enemy_party.size() - 1) {
        selection_2 = 0;
      }
    } else if (input.keyPressed("up") > 0) {
      sound.playSound("select_sound", 1);
      selection_2 -= 1;
      if (selection_2 < 0) {
        selection_2 = enemy_party.size() - 1;
      }
    } else if (input.keyPressed("a") > 0) {
      sound.playSound("accept_sound", 1);
      // choose the choice!
      mode = "acting";
      selection_character->target = enemy_party[selection_2];
      selection_character->startAttack();
    }
  }
}

void Battlin::render() {
  state->map->draw(-state->camera_x, -state->camera_y);

  if (mode == "prep") {
    for (BattleCharacter* character : everyone) {
      character->drawPrepMode();
    }
  }

  if (mode == "charging" || mode == "selecting" || mode == "acting" || mode == "finished") {
    for (BattleCharacter* character : everyone) {
      if (character != acting_enemy && character != selection_character) {
        character->drawActiveMode();
      }
    }
    // draw the action character last so the actions are on top
    if (selection_character != NULL) {
      selection_character->drawActiveMode();
      if (effects.busy(selection_character->unique_name + "_attack_hold_move")) {
        damage_text->setText(to_string(selection_character->damage_value));
        damage_text->draw({selection_character->target->battle_x - selection_character->target->direction * 50, selection_character->target->battle_y + selection_character->target->margin_y});
      }
    }
    if (acting_enemy != NULL) {
      acting_enemy->drawActiveMode();
      if (effects.busy(acting_enemy->unique_name + "_attack_hold_move")) {
        damage_text->setText(to_string(acting_enemy->damage_value));
        damage_text->draw({acting_enemy->target->battle_x - acting_enemy->target->direction * 50, acting_enemy->target->battle_y + acting_enemy->target->margin_y});
      }
    }

  }

  if (mode == "charging" || mode == "selecting" || mode == "acting") {
    drawInfoCard();
  }

  if (mode == "selecting") {
    drawSelection();
  }

  state->map->overlayer(-state->camera_x, -state->camera_y);

  bool player_defeated = true;
  for (BattleCharacter* character : player_party) {
    if (character->hp > 0) player_defeated = false;
  }
  if (mode == "finished") {
    black_screen->draw();
  }
}

void Battlin::drawInfoCard() {
  // Set the card lines
  vector<string> lines = {};
  for (BattleCharacter* character : player_party) {
    lines.push_back(printLine({
      character->display_name, 
      to_string(character->hp) + "/" + to_string(character->max_hp),
      "12/12", 
      ""}, {11, 8, 6, 20})
    );
  }
  info_card->setTextLines(lines);

  // Draw the header and card
  header->draw();
  info_card->draw();

  // Draw the gauge outlines and gauge fills
  graphics.setColor("#FFFFFF", 1.0);
  int count = 0;
  for (BattleCharacter* character : player_party) {
    graphics.drawImage(
      "charge_gauge_outline",
      hot_config.getInt("menu", "battle_gauge_outline_x"),
      hot_config.getInt("menu", "battle_gauge_outline_y") + hot_config.getInt("menu", "battle_gauge_spacing") * count,
      true, 0, 1
    );

    string fill_color = "orange";
    if (character->ap == character->max_ap) fill_color = "green";
    graphics.drawImage(
      "charge_gauge_" + fill_color + "_fill",
      hot_config.getInt("menu", "battle_gauge_outline_x") + hot_config.getInt("menu", "battle_gauge_fill_margin_x"),
      hot_config.getInt("menu", "battle_gauge_outline_y") + hot_config.getInt("menu", "battle_gauge_fill_margin_y") + hot_config.getInt("menu", "battle_gauge_spacing") * count,
      true, 0, character->ap / character->max_ap, 1, 1
    );
    count++;
  }
}

void Battlin::drawSelection() {
  selection_card_1_header->draw();
  selection_card_1->draw();

  graphics.drawImage(
    "choice_arrow_2",
    hot_config.getInt("menu", "battle_choice_x"),
    hot_config.getInt("menu", "battle_choice_y") + selection_1 * hot_config.getInt("menu", "battle_choice_y_spacing"),
    true, 0, 1
  );

  if (selection_state == 2) {
    graphics.drawImage(
      "choice_hand",
      enemy_party[selection_2]->battle_x + (good_direction == -1 ? 40 : -60),
      enemy_party[selection_2]->battle_y - 40,
      true, 0, (good_direction == -1 ? 1 : -1), 1, 1
    );
  }
}

string Battlin::printLine(vector<string> information, vector<int> widths) {
  string result = "";
  for (int i = 0; i < information.size(); i++) {
    result += information[i];
    string s(widths[i] - information[i].length(), ' ');
    result += s;
  }
  return result;
}

Battlin::~Battlin() {
}