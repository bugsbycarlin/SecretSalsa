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

  max_velocity = 7.0;
  max_ax = 0.75;
  max_ay = 0.65;
  velocity_decay = 0.87;
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

void Character::walkBehavior(float ax, float ay) {
  if (ax < -1 * max_ax) ax = -1 * max_ax;
  if (ax > max_ax) ax = max_ax;
  if (ay < -1 * max_ay) ay = -1 * max_ay;
  if (ay > max_ay) ay = max_ay;

  vx += ax;
  vy += ay;

  if (vx > max_velocity) vx = max_velocity;
  if (vx < -max_velocity) vx = -max_velocity;
  if (vy > max_velocity) vy = max_velocity;
  if (vy < -max_velocity) vy = -max_velocity;

  position old_position = {x, y};

  if (state->map->checkPath(x + vx, y + vy)) {
    x += vx;
    y += vy;
  } else {
    bool done = false;
    vector<float> angles = {
      velocity_tolerance / 10.0f,
      -velocity_tolerance / 10.0f,
      velocity_tolerance / 3.0f,
      -velocity_tolerance / 3.0f,
      velocity_tolerance / 1.5f,
      -velocity_tolerance / 1.5f,
      velocity_tolerance,
      -velocity_tolerance
    };
    for (float angle : angles) {
      point p = math_utils.rotateVector(vx, vy, angle);
      if (!done && state->map->checkPath(x + p.x, y + p.y)) {
        x += p.x;
        y += p.y;
        vx = restitution * p.x;
        vy = restitution * p.y;
        done = true;
      }
    }
  }
  
  vx *= velocity_decay;
  vy *= velocity_decay;
  if (abs(vx) > 0.1) {
    if (vx >= 0) {
      direction = 1;
    } else {
      direction = -1;
    }
  }

  if (abs(vx) <= 0.01) vx = 0;
  if (abs(vy) <= 0.01) vy = 0;

  if (x != old_position.x || y != old_position.y) {
    position_history.push_front(old_position);
    if (position_history.size() > history_size) {
      position_history.pop_back();
    }
  }
}

void Character::followBehavior(int x, int y) {
  int old_x = this->x;
  int old_y = this->y;
  this->x = x;
  this->y = y;
  direction = (x >= old_x) ? 1 : -1;
  vx = x - old_x;
  vy = y - old_y;

  if (x != old_x || y != old_y) {
    position_history.push_front({old_x, old_y});
    if (position_history.size() > history_size) {
      position_history.pop_back();
    }
  }
}

void Character::seekBehavior(int x, int y) {
  int old_x = this->x;
  int old_y = this->y;
  // 30 is an arbitrary number
  float distance = math_utils.distance(x, y, old_x, old_y);
  if (distance > 20) {
    walkBehavior((x - old_x) / distance, (y - old_y) / distance);
  }
}

void Character::koBehavior() {
  string k = "ko_countdown_" + unique_name;
  if (!timing.check(k)) {
    timing.mark(k);
  }
  if (timing.since(k) > hot_config.getFloat("game", "ko_countdown")) {
    hp = max_hp;
    timing.remove(k);
  }
}

void Character::battlePrepBehavior() {
  if (battle_x < battle_home_x) {
    battle_x += max_velocity / 2.0;
  } else if (battle_x > battle_home_x) {
    battle_x -= max_velocity / 2.0;
  }
  if (abs(battle_home_x - battle_x) < max_velocity / 2.0) battle_x = battle_home_x;
  if (battle_y < battle_home_y) {
    battle_y += max_velocity / 2.0;
  } else if (battle_y > battle_home_y) {
    battle_y -= max_velocity / 2.0;
  }
  if (abs(battle_home_y - battle_y) < max_velocity / 2.0) battle_y = battle_home_y;
}

void Character::simpleBounceAnimation() {
  //effects.makeOscillation("simple_bounce_walk_" + unique_name, hot_config.getInt("animation", "walk_bounce_height"), hot_config.getFloat("animation", "walk_bounce_speed"));
  effects.makeTween("simple_bounce_walk_" + unique_name, 0, 2 * hot_config.getInt("animation", "walk_bounce_height"), hot_config.getFloat("animation", "walk_bounce_speed"));
  effects.start("simple_bounce_walk_" + unique_name);
  effects.makeOscillation("simple_bounce_lean_" + unique_name, hot_config.getInt("animation", "walk_bounce_lean"), 2 * hot_config.getFloat("animation", "walk_bounce_speed"));
  effects.start("simple_bounce_lean_" + unique_name);
}

void Character::walkAnimation() {
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
  // printf("Drawing %s\n", animations["static"][0].c_str());
  int bounce_y = 0;
  int rotation = 0;
  if (hp <= 0 && animations.count("ko") == 1) {
    setAnimation("ko");
    setFrame(0);
  }
  else if ((timing.check("simple_bounce_walk_" + unique_name) && (abs(vx) > 0.01 || abs(vy) > 0.01))
    || animations["static"][0] == "robin") {
    //bounce_y = effects.oscillation("simple_bounce_walk_" + unique_name);
    bounce_y = effects.tween("simple_bounce_walk_" + unique_name, effects.CUBIC);
    if (bounce_y > hot_config.getInt("animation", "walk_bounce_height")) {
      bounce_y = 2 * hot_config.getInt("animation", "walk_bounce_height") - bounce_y;
    }
    rotation = effects.oscillation("simple_bounce_lean_" + unique_name);
    walkAnimation();
  } else {
    setAnimation("static");
    setFrame(0);
  }
  if (effects.finished("simple_bounce_walk_" + unique_name)) {
    effects.start("simple_bounce_walk_" + unique_name);
  }
  graphics.drawImage(
    animations[current_animation][current_frame],
    x - state->camera_x + margin_x,
    y - state->camera_y + margin_y - bounce_y,
    true, rotation, 1, direction == 1 ? true : false, false, false
  );
}

void Character::battlePrepDraw() {
  int bounce_y = effects.tween("simple_bounce_walk_" + unique_name, effects.CUBIC);
  if (bounce_y > hot_config.getInt("animation", "walk_bounce_height")) {
    bounce_y = 2 * hot_config.getInt("animation", "walk_bounce_height") - bounce_y;
  }
  int rotation = effects.oscillation("simple_bounce_lean_" + unique_name);
  walkAnimation();

  if (effects.finished("simple_bounce_walk_" + unique_name)) {
    effects.start("simple_bounce_walk_" + unique_name);
  }

  graphics.drawImage(
    animations[current_animation][current_frame],
    battle_x + margin_x,
    battle_y + margin_y - bounce_y,
    true, rotation, 1, direction == 1 ? true : false, false, false
  );
}

void Character::battleDraw() {
  setAnimation("static");
  setFrame(0);

  graphics.drawImage(
    animations[current_animation][current_frame],
    battle_x + margin_x,
    battle_y + margin_y,
    true, 0, 1, direction == 1 ? true : false, false, false
  );
}


Character::~Character() {
}