/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "walkingcharacter.h"

using namespace std;
using namespace Honey;

WalkingCharacter::WalkingCharacter(State* state) : Character(state) {
  max_velocity = 7.0;
  max_ax = 0.75;
  max_ay = 0.65;
  velocity_decay = 0.87;

  vx = 0;
  vy = 0;
}

void WalkingCharacter::walkBehavior(float ax, float ay) {
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

void WalkingCharacter::followBehavior(int x, int y) {
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

void WalkingCharacter::seekBehavior(int x, int y) {
  int old_x = this->x;
  int old_y = this->y;
  // 30 is an arbitrary number
  float distance = math_utils.distance(x, y, old_x, old_y);
  if (distance > 20) {
    walkBehavior((x - old_x) / distance, (y - old_y) / distance);
  }
}

void WalkingCharacter::koBehavior() {
  string k = "ko_countdown_" + unique_name;
  if (!timing.check(k)) {
    timing.mark(k);
  }
  if (timing.since(k) > hot_config.getFloat("game", "ko_countdown")) {
    hp = max_hp;
    timing.remove(k);
  }
}

void WalkingCharacter::draw() {
  // printf("Drawing %s\n", animations["static"][0].c_str());
  if (hp <= 0 && animations.count("ko") == 1) {
    setAnimation("ko");
    setFrame(0);
  } else if (abs(vx) > 0.01 || abs(vy) > 0.01 || animations["static"][0] == "robin") {
    walkAnimation();
  } else {
    setAnimation("static");
    setFrame(0);
  }

  graphics.drawImage(
    animations[current_animation][current_frame],
    x - state->camera_x + margin_x,
    y - state->camera_y + margin_y - bounce_y,
    true, rotation, direction, 1, 1
  );
}

WalkingCharacter::~WalkingCharacter() {
}