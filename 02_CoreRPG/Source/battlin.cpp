/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "battlin.h"

using namespace std;
using namespace Honey;

// Simple Debug Print
void db(int x) {
  printf("Here %d\n", x);
}

Battlin::Battlin(State* state, vector<WalkingCharacter*> player_party, vector<WalkingCharacter*> enemy_party) {
  this->state = state;
  good_direction = 1;
  root_player_party = player_party;
  root_enemy_party = enemy_party;
}

void Battlin::initialize() {
  mode = "prep";

  ap_delay = hot_config.getFloat("game", "ap_delay");
  if (state->has("slow_battles")) {
    ap_delay *= 2;
  }

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

  battle_frame_counter = 0;

  selection_character = NULL;
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

    if (state->has("all_blind")) {
      bc->status_effects["blind"] = true;
    }

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

    if (state->has("all_blind")) {
      bc->status_effects["blind"] = true;
    }

    if (bc->hp <= 0) bc->hp = 1;

    position p = layouts.tile("battle_" + good_place + "_layout", bc->battle_column, bc->battle_row);
    bc->battle_x = g->x - state->camera_x;
    bc->battle_y = g->y - state->camera_y;
    bc->battle_home_x = p.x + -1 * good_direction * hot_config.getInt("layout", "battle_layout_tilt") * bc->battle_row;
    bc->battle_home_y = p.y;

    player_party.push_back(bc);
    everyone.push_back(bc);
    j += 1;
  }

  for (BattleCharacter* character : everyone) {
    if (character->hp <= 0 && character->action_state != "ko") {
      character->action_state = "ko";
    }
  }
}

void Battlin::initializeMenus() {
  header = new Menu("Art/", "menu", "battle_header");
  string s = printLine({"Name", "HP", "SP", "Charge"}, {11, 8, 6, 20});
  header->setTextLines({s});

  info_card = new Menu("Art/", "menu", "battle_info_card");

  selection_card_1_header = new Menu("Art/", "menu", "selection_card_1_header");
  selection_card_1_header->setTextLines({"Action"});

  selection_card_1 = new Menu("Art/", "menu", "selection_card_1");
  selection_card_1->setTextLines({"Attack", "Special", "Item", "Move"});

  selection_card_2_header = new Menu("Art/", "menu", "selection_card_2_header");
  selection_card_2_header->setTextLines({" "});

  selection_card_2 = new Menu("Art/", "menu", "selection_card_2");
  selection_card_2->setTextLines({" "});
}

void Battlin::logic() {
  //printf("Mode is %s\n", mode.c_str());
  if (mode == "prep") {
    if (!timing.check("music_down")) timing.mark("music_down");
    if (state->get("switch_music_for_battle") == 1) {
      if (timing.since("music_down") < 1) {
        float volume = 1.0;
        if (volume > hot_config.getFloat("music", "music_volume")) {
          volume = hot_config.getFloat("music", "music_volume");
        }
        sound.setMusicVolume(volume - timing.since("music_down"));
      }
    }

    everybodyGetInPlace(); // mode might change to action

    if (mode == "action") { // it was just switched
      if (state->get("switch_music_for_battle") == 1) {
        sound.stopMusic();
        sound.playMusic("battle_fanfare", -1);
        float volume = 1.0;
        if (volume > hot_config.getFloat("music", "music_volume")) {
          volume = hot_config.getFloat("music", "music_volume");
        }
        sound.setMusicVolume(volume);
      }
    }
  }

  if (mode == "action") {
    if (selection_character != NULL && selection_character->hp <= 0) {
      selection_character = NULL;
      timing.mark("done_selecting");
    }

    // if (selection_character != NULL && selection_character->action_state != "choosing") {
    //   selection_character = NULL;
    // }

    for (BattleCharacter* character : everyone) {
      if (character->action_state == "acting") {
        character->continueAttack();
      }

      if (character->action_state == "skill" && character->skill == "Magic") {
        character->continueMagic();
      }

      if (character->action_state == "walking" && (character->battle_home_x != character->battle_x
        || character->battle_home_y != character->battle_y)) {
        character->walkToStartingPosition();
      } else if (character->action_state == "walking") {
        character->action_state = "charging";
        character->ap = 0;
      }
    }

    for (BattleCharacter* character : everyone) {
      if (character->hp <= 0 && character->action_state != "ko") {
        character->action_state = "ko";
        sound.playSound(hot_config.getString("game", character->name + "_ko_sound"), 1);
      }
    }

    chargeGauges();

    bool someone_acting = false;
    for (BattleCharacter* character : everyone) {
      // || character->action_state == "walking"
      if (character->action_state == "acting" || character->action_state == "skill") {
        someone_acting = true;
      }
    }
    bool enemy_free_to_act = true;
    if (hot_config.getString("game", "battle_pace") == "one" && someone_acting) {
      enemy_free_to_act = false;
    }

    for (BattleCharacter* character : everyone) {
      if (character->action_state == "ready" && character->hp > 0) {
        if (!character->player_character) {
          if (enemy_free_to_act) {
            character->startAutomaticBattle(player_party);
          }
        } else {
          if (state->get("tune_bear_berserk") && character->name == "tune_bear") {
            character->startAutomaticBattle(enemy_party);
          } else {
            character->action_state = "waiting";
            action_queue.push(character);
          }
        }
      }
    }

    if (selection_character == NULL && action_queue.size() > 0 && timing.since("done_selecting") > 0.1) {
      selection_character = action_queue.front();
      action_queue.pop();
      selection_character->action_state = "choosing";
      selection_1 = 0;
      selection_2 = 0;
      selection_3 = 0;
      selection_row = 0;
      selection_column = 0;
      selection_state = 1;
      selection_card_1->setTextLines({"Attack", selection_character->skill, "Item", "Move"});
      if (selection_character->skill == "Tapes") {
        selection_card_1->setLineColor(1, "#CCCCCC");
      } else {
        selection_card_1->setLineColor(1, "#000000");
      }
    }
  }

  if (selection_character != NULL) {
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

  bool someone_acting = false;
  for (BattleCharacter* character : everyone) {
    if (character->action_state == "acting" || character->action_state == "skill" || character->action_state == "walking") {
      someone_acting = true;
    }
  }
  if (mode != "finished" && (player_defeated || enemy_defeated) && !someone_acting) {
    cleanUpEffects();
    mode = "finished";
    timing.mark("finished_battlin");
    black_screen->setOpacity(1.0, hot_config.getFloat("game", "battle_finish_time"));
    timing.mark("music_down");
  }

  if (state->get("switch_music_for_battle") == 1 || player_defeated) {
    if (mode == "finished" && timing.check("music_down")) {
      if (timing.since("music_down") < 1) {
        float volume = 1.0;
        if (volume > hot_config.getFloat("music", "music_volume")) {
          volume = hot_config.getFloat("music", "music_volume");
        }
        sound.setMusicVolume(volume - timing.since("music_down"));
      }
    }
  }

  if (mode == "finished" && timing.since("finished_battlin") > hot_config.getFloat("game", "battle_finish_time")) {
    if (!player_defeated) {
      // Move HP into permanent characters.
      for (BattleCharacter* character : player_party) {
        character->permanent_character->hp = character->hp;
        character->permanent_character->sp = character->sp;
      }
      // Get out of battle
      state->modes.pop();
    } else {
      state->modes = {}; // huge leak
      Talkin* x = new Talkin(state);
      state->modes.push(x);
      x->initialize();
      x->conversation->setCurrentConversation("training_failure");
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
      mode = "action";
    }
}

void Battlin::chargeGauges() {
  if (!timing.check("ap_delay")) {
      timing.mark("ap_delay");
    } else if (timing.since("ap_delay") > ap_delay) {
      for (BattleCharacter* character : everyone) {
        if (character->action_state == "charging" && character->hp > 0) {
          character->ap += character->ap_rate;

          // If a character is facing the wrong way, when the gauge is charged, turn around and reset the gauge.
          if (character->ap >= character->max_ap && character->battle_direction != character->direction) {
            character->direction = character->battle_direction;
            character->ap = 0;
          }

          if (character->ap >= character->max_ap) {
            character->ap = character->max_ap;
            character->action_state = "ready";
          }
        }
      }
      timing.mark("ap_delay");
    }
}

void Battlin::handleSelection() {
  if (selection_state == 1) {
    if (input.actionPressed("down") > 0) {
      sound.playSound("select_sound", 1);
      selection_1 += 1;
      if (selection_1 > selection_1_max) {
        selection_1 = 0;
      }
    } else if (input.actionPressed("up") > 0) {
      sound.playSound("select_sound", 1);
      selection_1 -= 1;
      if (selection_1 < 0) {
        selection_1 = selection_1_max;
      }
    } else if (input.actionPressed("accept") > 0 && !(selection_character->skill == "Tapes" && selection_1 == 1)) {
      sound.playSound("accept_sound", 1);
      // choose the choice!
      if (selection_1 == 0 ||
        (selection_1 == 1 && selection_character->skill == "Forage")) {
        selection_state = 2;
        for (int i = 0; i < enemy_party.size() - 1; i++) {
          if (enemy_party[selection_2]-> hp <= 0) {
            selection_2 += 1;
          }
        }
      } else if (selection_1 == 1 && selection_character->skill_list[0] != "n/a") {
        printf("Skill list has length %lu\n", selection_character->skill_list.size());
        printf("Skill 1: %s\n", selection_character->skill_list[0].c_str());
        selection_state = 2;
        selection_card_2_header->setTextLines({selection_character->skill});
        selection_card_2->setTextLines(selection_character->skill_list);
        for(int i = 0; i < selection_character->skill_list.size(); i++) {
          int cost = hot_config.getInt("game", selection_character->skill_list[i] + "_cost");
          if (selection_character->sp < cost) {
            selection_card_2->setLineColor(i, "#CCCCCC");
          } else {
            selection_card_2->setLineColor(i, "#000000");
          }
        }
      } else if (selection_1 == 2) {
        selection_state = 2;
        selection_card_2_header->setTextLines({"Item"});
        vector<string> item_strings;
        for (int i = 0; i < state->item_counts.size(); i++) {
          item_strings.push_back(to_string(state->item_counts[i]) + " " + state->item_names[i]);
        }
        selection_card_2->setTextLines(item_strings);
        for (int i = 0; i < state->item_counts.size(); i++) {
          if (state->item_counts[i] <= 0) {
            selection_card_2->setLineColor(i, "#CCCCCC");
          } else {
            selection_card_2->setLineColor(i, "#000000");
          }
        }
      } else if (selection_1 == 3) {
        selection_state = 2;
        selection_row = selection_character->battle_row;
        selection_column = selection_character->battle_column;
      } else {
        selection_character->ap = 0;
        selection_character->action_state = "charging";
        timing.mark("done_selecting");
        selection_character = NULL;
      }
    } else if (input.actionPressed("cancel") > 0) {
      selection_character->ap = 0;
      selection_character->action_state = "charging";
      timing.mark("done_selecting");
      selection_character = NULL;
    }
  } else if (selection_state == 2 && selection_1 == 3) {
    if (input.actionPressed("left") > 0) {
      sound.playSound("select_sound", 1);
      selection_column -= 1;
      if (selection_column < 0) {
        selection_column = 1;
      }
    } else if (input.actionPressed("right") > 0) {
      sound.playSound("select_sound", 1);
      selection_column += 1;
      if (selection_column > 1) {
        selection_column = 0;
      }
    } else if (input.actionPressed("up") > 0) {
      sound.playSound("select_sound", 1);
      selection_row -= 1;
      if (selection_row < 0) {
        selection_row = 3;
      }
    } else if (input.actionPressed("down") > 0) {
      sound.playSound("select_sound", 1);
      selection_row += 1;
      if (selection_row > 3) {
        selection_row = 0;
      }
    } else if (input.actionPressed("cancel") > 0) {
      selection_state = 1;
    } else if (input.actionPressed("accept") > 0) {
      bool occupied = false;
      for (BattleCharacter* character : player_party) {
        if (character->battle_row == selection_row && character->battle_column == selection_column) {
          occupied = true;
        }
      }

      if (!occupied) {
        sound.playSound("accept_sound", 1);
        string good_place = "left";
        if (good_direction == -1) {
          good_place = "right";
        }  
        selection_character->battle_row = selection_row;
        selection_character->battle_column = selection_column;

        selection_character->addBounceAnimation();

        position p = layouts.tile("battle_" + good_place + "_layout", selection_character->battle_column, selection_character->battle_row);
        selection_character->battle_home_x = p.x + -1 * good_direction * hot_config.getInt("layout", "battle_layout_tilt") * selection_character->battle_row;
        selection_character->battle_home_y = p.y;
        selection_character->startWalk();

        selection_character = NULL;
      }
    }
  } else if (selection_state == 2 && (selection_1 == 0 || (selection_1 == 1 && selection_character->skill == "Forage"))) {
    if (input.actionPressed("down") > 0) {
      sound.playSound("select_sound", 1);
      selection_2 += 1;
      if (selection_2 > enemy_party.size() - 1) {
        selection_2 = 0;
      }
    } else if (input.actionPressed("up") > 0) {
      sound.playSound("select_sound", 1);
      selection_2 -= 1;
      if (selection_2 < 0) {
        selection_2 = enemy_party.size() - 1;
      }
    } else if (input.actionPressed("accept") > 0) {
      sound.playSound("accept_sound", 1);
      // choose the choice!
      if ((selection_1 == 1 && selection_character->skill == "Forage")) {
        selection_character->attack_using_skill = true;
      } else {
        selection_character->attack_using_skill = false;
      }
      selection_character->target = enemy_party[selection_2];
      selection_character->startAttack();
      selection_character = NULL;
    } else if (input.actionPressed("cancel") > 0) {
      selection_state = 1;
    }
  } else if (selection_state == 2 && selection_1 == 1) {
    if (input.actionPressed("down") > 0) {
      sound.playSound("select_sound", 1);
      selection_2 += 1;
      if (selection_2 > selection_character->skill_list.size() - 1) {
        selection_2 = 0;
      }
    } else if (input.actionPressed("up") > 0) {
      sound.playSound("select_sound", 1);
      selection_2 -= 1;
      if (selection_2 < 0) {
        selection_2 = selection_character->skill_list.size() - 1;
      }
    } else if (input.actionPressed("accept") > 0) {
      int cost = hot_config.getInt("game", selection_character->skill_list[selection_2] + "_cost");
      if (selection_character->sp >= cost) {
        sound.playSound("accept_sound", 1);
        selection_state = 3;
        // Move this out somewhere.
        if (selection_character->skill_list[selection_2] == "Ice"
          || selection_character->skill_list[selection_2] == "Dust") {
          selection_3 = -1;
        }
      }
    } else if (input.actionPressed("cancel") > 0) {
      selection_state = 1;
    }
  } else if (selection_state == 2 && selection_1 == 2) {
    if (input.actionPressed("down") > 0) {
      sound.playSound("select_sound", 1);
      selection_2 += 1;
      if (selection_2 > state->item_counts.size() - 1) {
        selection_2 = 0;
      }
    } else if (input.actionPressed("up") > 0) {
      sound.playSound("select_sound", 1);
      selection_2 -= 1;
      if (selection_2 < 0) {
        selection_2 = state->item_counts.size() - 1;
      }
    } else if (input.actionPressed("accept") > 0) {
      if (state->item_counts[selection_2] > 0) {
        selection_state = 3;
        selection_3 = 0;
      }
    } else if (input.actionPressed("cancel") > 0) {
      selection_state = 1;
    }
  } else if (selection_state == 3 && selection_1 == 1 && selection_character->skill == "Magic") {
    if (input.actionPressed("accept") > 0) {
      sound.playSound("accept_sound", 1);
      int cost = hot_config.getInt("game", selection_character->skill_list[selection_2] + "_cost");
      selection_character->sp -= cost;
      if (selection_3 == -1) {
        selection_character->magic_targets = enemy_party;
      }
      selection_character->current_magic = selection_character->skill_list[selection_2];
      selection_character->startMagic();
      selection_character = NULL;
      timing.mark("done_selecting");
    } else if (input.actionPressed("cancel") > 0) {
      selection_state = 2;
    }
  } else if (selection_state == 3 && selection_1 == 2) {
    if (input.actionPressed("down") > 0) {
      sound.playSound("select_sound", 1);
      selection_3 += 1;
      if (selection_3 > player_party.size() - 1) {
        selection_3 = 0;
      }
    } else if (input.actionPressed("up") > 0) {
      sound.playSound("select_sound", 1);
      selection_3 -= 1;
      if (selection_3 < 0) {
        selection_3 = player_party.size() - 1;
      }
    } else if (input.actionPressed("accept") > 0) {
      sound.playSound("accept_sound", 1);
      state->item_counts[selection_2] -= 1;
      if (state->item_counts[selection_2] < 0) state->item_counts[selection_2] = 0;
      effects.makeTween("battle_item_use", 1.0, 0.0, 1.0);
      effects.start("battle_item_use");
      item_name = state->item_names[selection_2];
      item_character = player_party[selection_3];
      if (item_character->hp > 0) {
        int delta = 0;
        if (item_name == "Chicken") {
          damage_text->setColor("#00bb00");
          if (item_character->name == "robin") {
            delta = 0;
          } else {
            delta = hot_config.getInt("game", "chicken_hp");
          }
          item_character->hp += delta;
          damage_text->setText(to_string(delta));
        } else if (item_name == "Coffee") {
          damage_text->setColor("#00bb00");
          damage_text->setText("+Speed");
          item_character->ap_rate *= 2;
        } else if (item_name == "Honey") {
          if (item_character->name == "witchycat") {
            damage_text->setColor("#0000bb");
            delta = hot_config.getInt("game", "honey_sp");;
            item_character->sp += delta;
          } else {
            damage_text->setColor("#00bb00");
            delta = hot_config.getInt("game", "honey_hp");
            item_character->hp += delta;
          }
          damage_text->setText(to_string(delta));
        }
      }

      if (item_character->hp > item_character->max_hp) {
        item_character->hp = item_character->max_hp;
      }
      if (item_character->sp > item_character->max_sp) {
        item_character->sp = item_character->max_sp;
      }

      selection_character->ap = 0;
      selection_character->action_state = "charging";
      timing.mark("done_selecting");
      selection_character = NULL;
    } else if (input.actionPressed("cancel") > 0) {
      selection_state = 2;
    }
  }
}

/*
Add cancel
Add dodge, so the second spell can maybe be dust / wind / blindness

*/

void Battlin::cleanUpEffects() {
  // Clean up from Ice
  state->map->ice_shards = false;
  state->map->rain_velocity = math_utils.rotateVector(0, 1, -1 * state->map->rain_angle);

  state->map->dust_storm = 0;
}

void Battlin::render() {
  battle_frame_counter += 1;

  state->map->draw(-state->camera_x, -state->camera_y);

  if (mode == "prep") {
    for (BattleCharacter* character : everyone) {
      character->drawPrepMode();
    }
  }

  if (mode == "action" || mode == "finished") {
    // This should be sorted by y value
    for (BattleCharacter* character : everyone) {
      character->drawActiveMode();
    }
  }

  state->map->overlayer(-state->camera_x, -state->camera_y);

  // I think this is a dummy to keep checking and eventually finish the effect
  if (effects.busy("battle_item_use")) {
    float x = effects.tween("battle_item_use", effects.LINEAR);
    if (state->string_values.count("battle_item_use") == 1 && state->getString("battle_item_use") != "") {
      item_name = state->getString("battle_item_use");
    }
    graphics.setColor("#FFFFFF", x);
    graphics.drawImage(
      item_name,
      640,
      50 + 50 * x,
      true, 0, 1
    );
  } else if (effects.finished("battle_item_use")) {
    item_character = NULL;
  }

  if (mode == "action" || mode == "finished") {
    // This should be sorted by y value
    for (BattleCharacter* character : everyone) {
      if (effects.busy(character->unique_name + "_attack_hold_move") &&
        !(character->attack_using_skill && character->skill == "Forage")) {
        if (character->damage_value >= 0) {
          damage_text->setColor("#bb0000");
          damage_text->setText(to_string(character->damage_value));
        } else {
          damage_text->setColor("#bb0000");
          damage_text->setText("Miss");
        }
        damage_text->draw({character->target->battle_home_x - character->target->direction * 50, character->target->battle_home_y + character->target->margin_y});
      }
      if (effects.busy("battle_item_use") && item_character != NULL && character == item_character) {
        damage_text->draw({character->battle_home_x + character->direction * 50, character->battle_home_y + character->margin_y});
      }
    }
  }

  if (mode == "action") {
    for (BattleCharacter* character : everyone) {
      if (character->action_state == "skill" 
        && character->skill == "Magic" 
        && effects.busy(character->unique_name + "_magic_effect")
        && character->damage_value >= 0) {
        damage_text->setText(to_string(character->damage_value));
        damage_text->setColor("#bb0000");
        for (BattleCharacter* target : character->magic_targets) {
          damage_text->draw({target->battle_home_x - target->direction * 50, target->battle_home_y + target->margin_y});
        }
      }
    }
  }

  if (mode == "action") {
    drawInfoCard();
  }

  if (selection_character != NULL && mode == "action") {
    drawSelection();
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
      to_string(character->sp) + "/" + to_string(character->max_sp), 
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
    if (character->hp > 0) {
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
    }
    count++;
  }

  // Draw the gauge outlines for enemies
  graphics.setColor("#FFFFFF", 1.0);
  for (BattleCharacter* character : enemy_party) {
    if (character->hp > 0 && character->action_state == "charging") {
      graphics.drawImage(
        "charge_gauge_outline",
        character->battle_home_x,
        character->battle_home_y,
        true, 0, 1
      );

      string fill_color = "orange";
      if (character->ap == character->max_ap) fill_color = "green";
      graphics.drawImage(
        "charge_gauge_" + fill_color + "_fill",
        character->battle_home_x + hot_config.getInt("menu", "battle_gauge_fill_margin_x"),
        character->battle_home_y + hot_config.getInt("menu", "battle_gauge_fill_margin_y"),
        true, 0, character->ap / character->max_ap, 1, 1
      );
    }
  }
}

void Battlin::drawSelection() {
  selection_card_1_header->draw();
  selection_card_1->draw();

  if (selection_state == 1) {
    graphics.drawImage(
      "choice_arrow_2",
      hot_config.getInt("menu", "battle_choice_x"),
      hot_config.getInt("menu", "battle_choice_y") + selection_1 * hot_config.getInt("menu", "battle_choice_y_spacing"),
      true, 0, 1
    );
  }

  if (selection_state == 2 && selection_1 == 3) {
    string good_place = "left";
    if (good_direction == -1) {
      good_place = "right";
    } 
    position p = layouts.tile("battle_" + good_place + "_layout", selection_column, selection_row);
    graphics.drawImage(
      "choice_hand",
      p.x - good_direction * hot_config.getInt("layout", "battle_layout_tilt") * selection_row + (good_direction == -1 ? -40 : 40),
      p.y - 40,
      true, 0, good_direction, 1, 1
    );
  }

  if ((selection_state == 2 && selection_1 == 0) ||
    (selection_state == 2 && selection_1 == 1 && selection_character->skill == "Forage")) {
    graphics.drawImage(
      "choice_hand",
      enemy_party[selection_2]->battle_x + (good_direction == -1 ? 40 : -60),
      enemy_party[selection_2]->battle_y - 40,
      true, 0, (good_direction == -1 ? 1 : -1), 1, 1
    );
  }

  if (selection_state >= 2 && (selection_1 == 1 || selection_1 == 2)) {
    if (selection_1 == 2 || selection_character->skill != "Forage") {
      selection_card_2_header->draw();
      selection_card_2->draw();

      graphics.drawImage(
        "choice_arrow_2",
        hot_config.getInt("menu", "battle_choice_x") + 100,
        hot_config.getInt("menu", "battle_choice_y") + selection_2 * hot_config.getInt("menu", "battle_choice_y_spacing"),
        true, 0, 1
      );
    }
  }

  if (selection_state == 3 && selection_1 == 2) {
    graphics.setColor("#FFFFFF", 1.0);
    graphics.drawImage(
      state->item_names[selection_2],
      player_party[selection_3]->battle_x + (good_direction == 1 ? 40 : -60),
      player_party[selection_3]->battle_y - 40,
      true, 0, (good_direction == -1 ? 1 : -1), 1, 1
    );
  }

  if (selection_state == 3 && selection_3 == -1 && battle_frame_counter % 2 == 1) {
    for (BattleCharacter* enemy : enemy_party) {
      graphics.drawImage(
        "choice_hand",
        enemy->battle_x + (good_direction == -1 ? 40 : -60),
        enemy->battle_y - 40,
        true, 0, (good_direction == -1 ? 1 : -1), 1, 1
      );
    }
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