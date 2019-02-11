/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "character.h"

using namespace std;
using namespace Honey;

int Character::unique_count = 0;

Character::Character(State* state) {
  this->state = state;

  unique_name = "character_" + to_string(unique_count);
  unique_count++;

  bounce_y = 0;
  rotation = 0;

  // For now
  direction = 1;
  x = hot_config.getInt("layout", "player_starting_x");
  y = hot_config.getInt("layout", "player_starting_y");
  margin_x = hot_config.getInt("layout", "player_margin_x");
  margin_y = hot_config.getInt("layout", "player_margin_y");
  addBounceAnimation();
}

void Character::addAnimation(string name, vector<string> animation) {
  animations[name] = animation;
}

void Character::setAnimation(string animation) {
  current_animation = animation;
}

void Character::setFrame(int frame) {
  current_frame = frame;
}


void Character::addBounceAnimation() {
  effects.makeTween("simple_bounce_walk_" + unique_name, 0, 2 * hot_config.getInt("animation", "walk_bounce_height"), hot_config.getFloat("animation", "walk_bounce_speed"));
  effects.start("simple_bounce_walk_" + unique_name);
  effects.makeOscillation("simple_bounce_lean_" + unique_name, hot_config.getInt("animation", "walk_bounce_lean"), 2 * hot_config.getFloat("animation", "walk_bounce_speed"));
  effects.start("simple_bounce_lean_" + unique_name);
}

void Character::walkAnimation() {
  if (timing.check("simple_bounce_walk_" + unique_name)) {
    bounce_y = effects.tween("simple_bounce_walk_" + unique_name, effects.CUBIC);
    if (bounce_y > hot_config.getInt("animation", "walk_bounce_height")) {
      bounce_y = 2 * hot_config.getInt("animation", "walk_bounce_height") - bounce_y;
    }
    rotation = effects.oscillation("simple_bounce_lean_" + unique_name);
  }

  if (effects.finished("simple_bounce_walk_" + unique_name)) {
    effects.start("simple_bounce_walk_" + unique_name);
  }

  if (animations.count("walking") == 1 && animations["walking"].size() > 1) {
    if (!timing.check("walk_animation_" + unique_name) 
      || timing.since("walk_animation_" + unique_name) > walk_animation_speed) {
      timing.mark("walk_animation_" + unique_name);
      current_frame += 1;
      if (current_frame >= animations["walking"].size()) current_frame = 0;
      setAnimation("walking");
      setFrame(current_frame);
    }
  } else {
    setAnimation("static");
    setFrame(0);
  }
}

void Character::draw() {
}

void Character::cloneFromPermanentCharacter(PermanentCharacter* character) {
  name = character->name;
  display_name = character->display_name;

  hp = character->hp;
  max_hp = character->max_hp;
  ap_rate = character->ap_rate;

  defense = character->defense;
  attack_min = character->attack_min;
  attack_max = character->attack_max;

  permanent_character = character;

  skill = character->skill;
}

Character::~Character() {
}