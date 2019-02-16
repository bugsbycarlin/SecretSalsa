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
  party = new WalkingParty(state);
  found_battle = false;
}

void Walkin::initializeWalkingParty() {
  for (PermanentCharacter* p : state->party) {
    WalkingCharacter* character = new WalkingCharacter(state);
    character->cloneFromPermanentCharacter(p);

    if (state->has("extra_hp")) {
      character->max_hp = hot_config.getInt("game", character->name + "_hp") + 30;
      character->hp = character->max_hp;
      character->permanent_character->max_hp = character->max_hp;
    }

    character->addAnimation("static", {p->name});
    character->addAnimation("ko", {p->name + "_ko"});
    character->addAnimation("hurt", {p->name + "_hurt"});
    if (p->name == "robin") {
      character->margin_y = hot_config.getInt("layout", "player_margin_y") - 50;
      character->addAnimation("walking", {"robin", "robin_flapping"});
    } else if (p->name == "witchycat") {
      character->addAnimation("casting", {"witchycat", "witchycat_speak"});
    }
    party->add(character);
  }
}

void Walkin::addBaddieParty() {
  WalkingParty* baddie_party = new WalkingParty(state);

  float max_velocity = 4.0 + 5.0 * (math_utils.randomInt(0, 10) / 10.0);
  float max_ax_multiplier = (0.8 + 0.4 * math_utils.randomInt(0, 10) / 10.0);
  float max_ay_multiplier = (0.8 + 0.4 * math_utils.randomInt(0, 10) / 10.0);

  int num_baddies_roll = math_utils.randomInt(0,20);
  int num_baddies = 2;
  if (num_baddies_roll > 15) {
    num_baddies = 3;
  } else if (num_baddies_roll > 12) {
    num_baddies = 1;
  }

  for (int i = 0; i < num_baddies; i++) {
    WalkingCharacter* baddie = new WalkingCharacter(state);
    baddie->cloneFromPermanentCharacter(state->enemy_templates["bigdog"]);

    baddie->max_hp = math_utils.randomInt(
      hot_config.getInt("game", "bigdog_hp") - 10,
      hot_config.getInt("game", "bigdog_hp") + 10
    );
    baddie->hp = baddie->max_hp;
    baddie->ap_rate = hot_config.getFloat("game", "bigdog_ap_rate") + 0.8 * (math_utils.randomInt(0,100) / 100.0);

    baddie->direction = 1;
    baddie->x = hot_config.getInt("layout", "baddie_starting_x");
    baddie->y = hot_config.getInt("layout", "baddie_starting_y");
    baddie->margin_x = hot_config.getInt("layout", "player_margin_x");
    baddie->margin_y = hot_config.getInt("layout", "player_margin_y");
    baddie->vx = 0;
    baddie->vy = 0;
    baddie->addAnimation("static", {"bigdog_static"});
    baddie->addAnimation("hurt", {"bigdog_hurt"});
    baddie->addAnimation("attacking", {"bigdog_kick"});
    baddie->addAnimation("walking", {"bigdog_walk_2_3", "bigdog_walk_2_4"});
    baddie->addAnimation("ko", {"bigdog_ko"});
    baddie->addBounceAnimation();
    baddie->walk_animation_speed = hot_config.getFloat("animation", "baddie_walking_speed");

    baddie->max_velocity = max_velocity;
    baddie->max_ax *= max_ax_multiplier;
    baddie->max_ay *= max_ay_multiplier;

    int item_roll = math_utils.randomInt(0,60);
    if (state->has("enemy_items")) {
      item_roll = math_utils.randomInt(0,30);
    }
    if (item_roll < 10) {
      baddie->item = "Chicken";
    } else if (item_roll < 20) {
      baddie->item = "Coffee";
    } else if (item_roll < 30) {
      baddie->item = "Honey";
    } else {
      baddie->item = "None";
    }

    baddie_party->add(baddie);
  }

  baddie_parties.push_back(baddie_party);
  baddie_seek_numbers.push_back(0);
  box* seek_box = new box();
  seek_box->top = lap_seek_points[0].top;
  seek_box->left = lap_seek_points[0].left;
  baddie_seek_points.push_back(seek_box);
}

void Walkin::initialize() {

  coach_bulldog_x = hot_config.getInt("layout", "coach_bulldog_x");
  coach_bulldog_y = hot_config.getInt("layout", "coach_bulldog_y");

  battle_trigger_distance = hot_config.getInt("game", "battle_trigger_distance");

  lap_count_box = new Menu("Art/", "menu", "lap_count");

  lap_zone_counter = 0;
  lap_zones = {
    {.top =  579, .left = 1520, .bottom = 622, .right = 1731},
    {.top =  222, .left = 869, .bottom = 429, .right = 974},
    {.top =  544, .left = 98, .bottom = 612, .right = 329},
    {.top =  739, .left = 1316, .bottom = 955, .right = 1409},
  };


  lap_seek_points = {
    {875, 853, 874, 912},
    {1494, 795, 1538, 843},
    {1628, 601, 1688, 598},
    {1536, 408, 1584, 358},
    {914, 330, 908, 268},
    {292, 397, 244, 366},
    {203, 585, 133, 594},
    {352, 817, 301, 858}
  };


  sound.setMusicVolume(hot_config.getFloat("music", "music_volume"));
  if (state->string_values.count("music_choice") != 1) {
    printf("No music chosen.\n");
    state->storeString("music_choice", "music_" + to_string(math_utils.randomInt(1,6)));
  }
  printf("This is the song: %s\n", hot_config.getString("music", state->getString("music_choice")).c_str());
  sound.playMusic(hot_config.getString("music", state->getString("music_choice")), -1);
  if (state->getString("music_choice") == "music_5") {
    state->store("switch_music_for_battle", 0);
  } else {
    state->store("switch_music_for_battle", 1);
  }

  if (state->getString("music_choice") == "music_1") {
    state->store("extra_hp", 1);
  } else if (state->getString("music_choice") == "music_2") {
    state->store("tune_bear_berserk", 1);
  } else if (state->getString("music_choice") == "music_3") {
    state->store("enemy_items", 1);
  } else if (state->getString("music_choice") == "music_4") {
    state->store("all_blind", 1);
  } else if (state->getString("music_choice") == "music_5") {
    state->store("slow_battles", 1);
  }
  
  state->item_counts = {};
  state->item_counts.push_back(hot_config.getInt("game", "chicken_start"));
  state->item_counts.push_back(hot_config.getInt("game", "coffee_start"));
  state->item_counts.push_back(hot_config.getInt("game", "honey_start"));
  state->item_names = {};
  state->item_names.push_back("Chicken");
  state->item_names.push_back("Coffee");
  state->item_names.push_back("Honey");

  initializeWalkingParty();
  addBaddieParty();

  state->camera_x = party->characters[0]->x - (state->screen_width / 2.0);
  state->camera_y = party->characters[0]->y - (state->screen_height / 1.5);

  wait_for_rain = math_utils.randomInt(
    hot_config.getInt("game", "rain_delay_min"),
    hot_config.getInt("game", "rain_delay_max"));
  timing.mark("wait_for_rain");

  timing.mark("walking_fade_in");
}

void Walkin::movementLogic() {
  float ax = 0;
  float ay = 0;

  if (input.keyDown("left") && input.keyDown("right")) {
  }
  else if (input.keyDown("left")) {
    ax = -1;
  }
  else if (input.keyDown("right")) {
    ax = 1;
  }

  if (input.keyDown("up") && input.keyDown("down")) {
  }
  else if (input.keyDown("up")) {
    ay = -1;
  }
  else if (input.keyDown("down")) {
    ay = 1;
  }

  party->walk(ax, ay);


  int px = party->characters[0]->x;
  int py = party->characters[0]->y;
  for (int i = 0; i < baddie_parties.size(); i++) {
    WalkingCharacter* baddie = (baddie_parties[i])->characters[0];
    if (baddie->hp > 0) {
      // if player is in the 200 pixel box ahead of baddie, seek player
      if ((baddie->direction == 1 && px >= baddie->x && px < baddie->x + 200 && py > baddie->y - 100 && py < baddie->y + 200)
      || (baddie->direction == -1 && px <= baddie->x && px > baddie->x - 200 && py > baddie->y - 100 && py < baddie->y + 200)) {
        baddie_parties[i]->seek(px, py);
      } else {
        box* target = baddie_seek_points[i];
        baddie_parties[i]->seek(target->top, target->left);
        if (math_utils.distance(target->top, target->left, baddie->x, baddie->y) < 30) {
          baddie_seek_numbers[i] += 1;
          if (baddie_seek_numbers[i] >= lap_seek_points.size()) baddie_seek_numbers[i] = 0;
          box* new_target = &lap_seek_points[baddie_seek_numbers[i]];
          float vector_x = new_target->bottom - new_target->top;
          float vector_y = new_target->right - new_target->left;
          float multiplier = (math_utils.randomInt(0, 20) - 10) / 10.0;
          target->top = new_target->top + vector_x * multiplier;
          target->left = new_target->left + vector_y * multiplier;
        }
      }
    }
  }
}

void Walkin::animationLogic() {

}

void Walkin::cameraLogic() {
  state->camera_target_x = party->characters[0]->x - (state->screen_width / 2.0);
  state->camera_target_y = party->characters[0]->y - (state->screen_height / 1.5);

  if (state->camera_x != state->camera_target_x) state->camera_x = state->camera_blend_factor * state->camera_x + (1 - state->camera_blend_factor) * state->camera_target_x;
  if (state->camera_y != state->camera_target_y) state->camera_y = state->camera_blend_factor * state->camera_y + (1 - state->camera_blend_factor) * state->camera_target_y;

  if (state->camera_x < 0) state->camera_x = 0;
  if (state->camera_x > state->map->map_width - state->screen_width) state->camera_x = state->map->map_width - state->screen_width;
  if (state->camera_y < 0) state->camera_y = 0;
  if (state->camera_y > state->map->map_height - state->screen_height) state->camera_y = state->map->map_height - state->screen_height;
}

void Walkin::gameLogic() {
  lap_count_box->setTextLines({
    to_string(state->get("laps")) + (state->get("laps") == 1 ? " Lap" : " Laps"),
    "Tune Bear " + to_string(state->party[0]->hp) + "/" + to_string(state->party[0]->max_hp),
    "Witchycat " + to_string(state->party[1]->hp) + "/" + to_string(state->party[1]->max_hp),
    "Robin " + to_string(state->party[2]->hp) + "/" + to_string(state->party[2]->max_hp)
  });

  // This happens if you survive battle. Comes after battle pop.
  if (found_battle) {
    found_battle = false;
    for (WalkingCharacter* character : current_battle_baddies->characters) {
      character->hp = 0;
    }
    for (WalkingCharacter* character : party->characters) {
      character->cloneFromPermanentCharacter(character->permanent_character);
    }
    timing.mark("post_battle_grace");
    if (state->get("switch_music_for_battle") == 1) {
      sound.playMusic(hot_config.getString("music", state->getString("music_choice")), -1);
      float volume = 1.0;
      if (volume > hot_config.getFloat("music", "music_volume")) {
        volume = hot_config.getFloat("music", "music_volume");
      }
      sound.setMusicVolume(volume);
    }
  }

  if (timing.since("wait_for_rain") > wait_for_rain 
    && state->map->raining == false
    && (!timing.check("post_battle_grace") || 
    timing.since("post_battle_grace") > hot_config.getFloat("game", "post_battle_grace_period"))) {
    state->map->startRain();
    sound.playSound("thunder", 1);
  }

  int px = party->characters[0]->x;
  int py = party->characters[0]->y;
  for (int i = 0; i < lap_zones.size(); i++) {
    if (lap_zone_counter == i && checkBox(px, py, lap_zones[i])) {
      lap_zone_counter += 1;
    }
    if (lap_zone_counter == 4) {
      lap_zone_counter = 0;
      sound.playSound("accept_sound", 1);
      state->store("laps", state->get("laps") + 1);
      addBaddieParty();

      if (state->get("laps") >= 10) {
        timing.mark("success_ending");
      }
    }
  }

  if (timing.check("success_ending") && timing.since("success_ending") >= 2.0) {
    state->modes = {}; // huge leak
    Talkin* x = new Talkin(state);
    state->modes.push(x);
    x->initialize();
    x->conversation->setCurrentConversation("training_success");
  }

  for (WalkingParty* baddie_party : baddie_parties) {
    for (WalkingCharacter* baddie : baddie_party->characters) {
      if (baddie->hp <= 0) {
        baddie->koBehavior();
      }
    }
  }

  if (!timing.check("success_ending")) {
    found_battle = false;
    if (!timing.check("post_battle_grace") || 
      timing.since("post_battle_grace") > hot_config.getFloat("game", "post_battle_grace_period")) {
      for (WalkingParty* baddie_party : baddie_parties) {
        for (WalkingCharacter* baddie : baddie_party->characters) {
          for (WalkingCharacter* goodie : party->characters) {
            if (baddie->hp > 0 && math_utils.distance(baddie->x, baddie->y, goodie->x, goodie->y) < battle_trigger_distance && !found_battle) {
              found_battle = true;
              // This will leak when it pops. Gotta delete it somewhere.
              Battlin* battle = new Battlin(state, party->characters, baddie_party->characters);
              current_battle_baddies = baddie_party;
              if (goodie->x >= baddie->x) {
                battle->good_direction = -1;
              } else {
                battle->good_direction = 1;
              }
              battle->initialize();
              state->modes.push(battle);
            }
          }
        }
      }
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

  for (WalkingParty* baddie_party : baddie_parties) {
    baddie_party->draw();
  }

  post_battle_counter += 1;
  if (!timing.check("post_battle_grace") ||
    timing.since("post_battle_grace") > hot_config.getFloat("game", "post_battle_grace_period")) {
    party->draw();
  } else {
    if (post_battle_counter % 2 == 0) {
      party->draw();
    }
  }

  graphics.drawImage(
    "coach_bulldog",
    coach_bulldog_x - state->camera_x,
    coach_bulldog_y - state->camera_y,
    true, 0, 1
  );

  state->map->overlayer(-state->camera_x, -state->camera_y);

  lap_count_box->draw();

  if (timing.since("walking_fade_in") <= 0.5) {
    int x = (int) -1280 * 2 * timing.since("walking_fade_in");
    int y = (int) 720.0 * 2 * timing.since("walking_fade_in");
    graphics.drawImage("black_screen", x, y);
  }

  // if (timing.check("post_battle_grace") && timing.since("post_battle_grace") < 1.0) {
  //   graphics.setColor("#ffffff", 1.0 - timing.since("post_battle_grace"));
  //   graphics.drawImage("black_screen", 0, 0);
  // }

  if (timing.check("success_ending")) {
    float opacity = timing.since("success_ending") / 2.0;
    if (opacity > 1.0) opacity = 1.0;
    graphics.setColor("#ffffff", opacity);
    graphics.drawImage("black_screen", 0, 0);
  }
}

Walkin::~Walkin() {
  delete lap_count_box;
}