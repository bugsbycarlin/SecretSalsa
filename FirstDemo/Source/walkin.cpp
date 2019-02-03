/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "walkin.h"

using namespace std;
using namespace Honey;


// Simple Debug Print
void db(int x) {
  printf("Here %d\n", x);
}

Walkin::Walkin(State* state) {
  this->state = state;
  party_group = new Group(state);
}

void Walkin::initializeCharacters() {
  Character* character = new Character(state);

  character->name = "tune_bear";
  character->display_name = "Tune Bear";
  character->max_hp = hot_config.getInt("game", "tune_bear_hp");
  character->hp = character->max_hp;character->direction = 1;
  character->x = hot_config.getInt("layout", "player_starting_x");
  character->y = hot_config.getInt("layout", "player_starting_y");
  character->margin_x = hot_config.getInt("layout", "player_margin_x");
  character->margin_y = hot_config.getInt("layout", "player_margin_y");
  character->vx = 0;
  character->vy = 0;
  character->addAnimation("static", {"tune_bear"});
  character->simpleBounceAnimation();
  character->ap_rate = 0.8 + 0.4 * (math_utils.randomInt(0,100) / 100.0);

  party_group->add(character);

  character = new Character(state);

  character->name = "witchycat";
  character->display_name = "Witchycat";
  character->max_hp = hot_config.getInt("game", "witchycat_hp");
  character->hp = character->max_hp;
  character->direction = 1;
  character->x = hot_config.getInt("layout", "player_starting_x");
  character->y = hot_config.getInt("layout", "player_starting_y");
  character->margin_x = hot_config.getInt("layout", "player_margin_x");
  character->margin_y = hot_config.getInt("layout", "player_margin_y");
  character->vx = 0;
  character->vy = 0;
  character->addAnimation("static", {"witchycat"});
  character->simpleBounceAnimation();
  character->ap_rate = 0.8 + 0.4 * (math_utils.randomInt(0,100) / 100.0);

  party_group->add(character);

  character = new Character(state);

  character->name = "witchycat2";
  character->display_name = "Witchycat 2";
  character->max_hp = hot_config.getInt("game", "witchycat_hp");
  character->hp = character->max_hp;
  character->direction = 1;
  character->x = hot_config.getInt("layout", "player_starting_x");
  character->y = hot_config.getInt("layout", "player_starting_y");
  character->margin_x = hot_config.getInt("layout", "player_margin_x");
  character->margin_y = hot_config.getInt("layout", "player_margin_y");
  character->vx = 0;
  character->vy = 0;
  character->addAnimation("static", {"witchycat"});
  character->simpleBounceAnimation();
  character->ap_rate = 0.8 + 0.4 * (math_utils.randomInt(0,100) / 100.0);

  party_group->add(character);

  character = new Character(state);

  character->name = "robin";
  character->display_name = "Robin";
  character->max_hp = hot_config.getInt("game", "robin_hp");
  character->hp = character->max_hp;
  character->direction = 1;
  character->x = hot_config.getInt("layout", "player_starting_x");
  character->y = hot_config.getInt("layout", "player_starting_y");
  character->margin_x = hot_config.getInt("layout", "player_margin_x");
  character->margin_y = hot_config.getInt("layout", "player_margin_y") - 50;
  character->vx = 0;
  character->vy = 0;
  character->addAnimation("static", {"robin"});
  character->addAnimation("walking", {"robin", "robin_flapping"});
  character->simpleBounceAnimation();
  character->walk_animation_speed = hot_config.getFloat("animation", "robin_flapping_animation_speed");
  printf("Robin animation speed: %0.2f\n", character->walk_animation_speed);
  character->ap_rate = 0.8 + 0.4 * (math_utils.randomInt(0,100) / 100.0);

  party_group->add(character);
}

void Walkin::addBaddieGroup() {
  Group* baddie_group = new Group(state);

  float max_velocity = 6.0 + 2.0 * (math_utils.randomInt(0, 10) / 10.0);
  float max_ax_multiplier = (0.8 + 0.4 * math_utils.randomInt(0, 10) / 10.0);
  float max_ay_multiplier = (0.8 + 0.4 * math_utils.randomInt(0, 10) / 10.0);

  int num_baddies_roll = math_utils.randomInt(0,20);
  int num_baddies = 1;
  if (num_baddies_roll > 17) {
    num_baddies = 3;
  } else if (num_baddies_roll > 13) {
    num_baddies = 2;
  }

  for (int i = 0; i < num_baddies; i++) {
    Character* baddie = new Character(state);

    baddie->name = "big_dog";
    baddie->display_name = "Big Dog";
    baddie->max_hp = math_utils.randomInt(
      hot_config.getInt("game", "baddie_min_hp"),
      hot_config.getInt("game", "baddie_max_hp")
    );
    baddie->hp = baddie->max_hp;

    baddie->direction = 1;
    baddie->x = hot_config.getInt("layout", "baddie_starting_x");
    baddie->y = hot_config.getInt("layout", "baddie_starting_y");
    baddie->margin_x = hot_config.getInt("layout", "player_margin_x");
    baddie->margin_y = hot_config.getInt("layout", "player_margin_y");
    baddie->vx = 0;
    baddie->vy = 0;
    baddie->addAnimation("static", {"baddie_static"});
    baddie->addAnimation("walking", {"baddie_walk_1", "baddie_walk_2"});
    baddie->addAnimation("ko", {"baddie_ko"});
    baddie->simpleBounceAnimation();
    baddie->walk_animation_speed = hot_config.getFloat("animation", "baddie_walking_speed");

    baddie->max_velocity = max_velocity;
    baddie->max_ax *= max_ax_multiplier;
    baddie->max_ay *= max_ay_multiplier;

    baddie->ap_rate = 0.8 + 0.4 * (math_utils.randomInt(0,100) / 100.0);

    baddie_group->add(baddie);
  }

  baddie_groups.push_back(baddie_group);
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

  lap_count_box = new Menu(
    hot_config.getInt("menu", "lap_count_x"),
    hot_config.getInt("menu", "lap_count_y"),
    hot_config.getInt("menu", "lap_count_width"),
    hot_config.getInt("menu", "lap_count_height"),
    "Art/",
    hot_config.getString("menu", "lap_count_image_root"),
    hot_config.getInt("menu", "lap_count_margin_x"),
    hot_config.getInt("menu", "lap_count_margin_y"),
    hot_config.getInt("menu", "lap_count_num_lines"),
    hot_config.getInt("menu", "lap_count_wrap_length"),
    hot_config.getBool("menu", "lap_count_typewriter"),
    hot_config.getFloat("menu", "lap_count_typewriter_delay"),
    hot_config.getString("menu", "lap_count_font_path"),
    hot_config.getInt("menu", "lap_count_font_size"),
    hot_config.getString("menu", "lap_count_font_color")
  );

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

  initializeCharacters();
  addBaddieGroup();

  state->camera_x = party_group->characters[0]->x - (state->screen_width / 2.0);
  state->camera_y = party_group->characters[0]->y - (state->screen_height / 1.5);
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

  party_group->walk(ax, ay);


  int px = party_group->characters[0]->x;
  int py = party_group->characters[0]->y;
  for (int i = 0; i < baddie_groups.size(); i++) {
    Character* baddie = (baddie_groups[i])->characters[0];
    if (baddie->hp > 0) {
      // if player is in the 200 pixel box ahead of baddie, seek player
      if ((baddie->direction == 1 && px >= baddie->x && px < baddie->x + 200 && py > baddie->y - 100 && py < baddie->y + 200)
      || (baddie->direction == -1 && px <= baddie->x && px > baddie->x - 200 && py > baddie->y - 100 && py < baddie->y + 200)) {
        baddie_groups[i]->seek(px, py);
      } else {
        box* target = baddie_seek_points[i];
        baddie_groups[i]->seek(target->top, target->left);
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
  state->camera_target_x = party_group->characters[0]->x - (state->screen_width / 2.0);
  state->camera_target_y = party_group->characters[0]->y - (state->screen_height / 1.5);

  if (state->camera_x != state->camera_target_x) state->camera_x = state->camera_blend_factor * state->camera_x + (1 - state->camera_blend_factor) * state->camera_target_x;
  if (state->camera_y != state->camera_target_y) state->camera_y = state->camera_blend_factor * state->camera_y + (1 - state->camera_blend_factor) * state->camera_target_y;

  if (state->camera_x < 0) state->camera_x = 0;
  if (state->camera_x > state->map->map_width - state->screen_width) state->camera_x = state->map->map_width - state->screen_width;
  if (state->camera_y < 0) state->camera_y = 0;
  if (state->camera_y > state->map->map_height - state->screen_height) state->camera_y = state->map->map_height - state->screen_height;
}

void Walkin::gameLogic() {
  lap_count_box->setText(to_string(state->get("laps")) + (state->get("laps") == 1 ? " LAP" : " LAPS"));

  if (input.keyPressed("r") > 0) {
    state->map->startRain();
  }
  if (input.keyPressed("s") > 0) {
    state->map->stopRain();
  }

  // if (input.keyPressed("b") > 0) {
  //   state->modes.push(new Battlin(state));
  // }

  int px = party_group->characters[0]->x;
  int py = party_group->characters[0]->y;
  for (int i = 0; i < lap_zones.size(); i++) {
    if (lap_zone_counter == i && checkBox(px, py, lap_zones[i])) {
      lap_zone_counter += 1;
    }
    if (lap_zone_counter == 4) {
      lap_zone_counter = 0;
      state->store("laps", state->get("laps") + 1);
      addBaddieGroup();
    }
  }

  for (Group* baddie_group : baddie_groups) {
    for (Character* baddie : baddie_group->characters) {
      if (baddie->hp <= 0) {
        baddie->koBehavior();
      }
    }
  }

  bool found_battle = false;
  for (Group* baddie_group : baddie_groups) {
    for (Character* baddie : baddie_group->characters) {
      for (Character* goodie : party_group->characters) {
        if (baddie->hp > 0 && math_utils.distance(baddie->x, baddie->y, goodie->x, goodie->y) < battle_trigger_distance && !found_battle) {
          found_battle = true;
          // This will leak when it pops. Gotta delete it somewhere.
          Battlin* battle = new Battlin(state);
          if (goodie->x >= baddie->x) {
            int i = 1;
            int j = 0;
            for (Character* baddie_b : baddie_group->characters) {
              battle->left_placements.push_back({baddie_b, i, j});
              j += 1;
            }
            i = 0;
            j = 0;
            for (Character* goodie_g : party_group->characters) {
              battle->right_placements.push_back({goodie_g, i, j});
              j += 1;
            }
          } else {
            int i = 1;
            int j = 0;
            for (Character* goodie_g : party_group->characters) {
              battle->left_placements.push_back({goodie_g, i, j});
              j += 1;
            }
            i = 0;
            j = 0;
            for (Character* baddie_b : baddie_group->characters) {
              battle->right_placements.push_back({baddie_b, i, j});
              j += 1;
            }
          }
          battle->party = party_group->characters;
          battle->initialize();
          state->modes.push(battle);
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

  for (Group* baddie_group : baddie_groups) {
    baddie_group->draw();
  }

  party_group->draw();

  graphics.drawImage(
    "coach_bulldog",
    coach_bulldog_x - state->camera_x,
    coach_bulldog_y - state->camera_y,
    true, 0, 1
  );

  state->map->overlayer(-state->camera_x, -state->camera_y);

  lap_count_box->draw();
}

Walkin::~Walkin() {
  delete lap_count_box;
}