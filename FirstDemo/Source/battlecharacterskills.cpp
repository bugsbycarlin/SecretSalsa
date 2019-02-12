/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "battlecharacter.h"

using namespace std;
using namespace Honey;

void BattleCharacter::startMagic() {
  if (skill != "Magic") {
    return;
  }

  action_state = "skill";

  effects.makeTween(unique_name + "_magic_incantation", 0, 100, magic_incantation_time);
  effects.start(unique_name + "_magic_incantation");

  printf("I have started the magic incantation for %s\n", current_magic.c_str());
}

void BattleCharacter::continueMagic() {
  if (skill != "Magic") {
    return;
  }

  if (hp <= 0) {
    // Finish up without doing magic
    printf("I got killed before I could do magic.\n");
    effects.remove(unique_name + "_magic_incantation");
    effects.remove(unique_name + "_magic_effect");
    if (current_magic == "Ice") {
      state->map->ice_shards = false;
      state->map->rain_velocity = math_utils.rotateVector(0, 1, -1 * state->map->rain_angle);
    }
    action_state = "charging";
    ap = 0;
  }

  // Dummy checks
  if (effects.busy(unique_name + "_magic_incantation")) {
    effects.tween(unique_name + "_magic_incantation", effects.LINEAR);
  }
  if (effects.busy(unique_name + "_magic_effect")) {
    effects.tween(unique_name + "_magic_effect", effects.LINEAR);
  }

  if (effects.finished(unique_name + "_magic_incantation")) {
    printf("Now I am switching to the effect.\n");
    effects.remove(unique_name + "_magic_incantation");
    effects.makeTween(unique_name + "_magic_effect", 0, 100, magic_effect_time);
    effects.start(unique_name + "_magic_effect");
    damage_value = -1;

    if (current_magic == "Ice") {
      if (state->map->raining) {
        // Good effect
        sound.playSound("ice_shatter", 1);
        state->map->ice_shards = true;
        state->map->rain_velocity = math_utils.rotateVector(0, 3.2, -1 * state->map->rain_angle);

        // int current_size = state->map->rain_field.size();
        // for (int i = 0; i < 2 * current_size; i++) {
        //   int target_x = math_utils.randomInt(0, state->map->map_width / 2.0);
        //   int target_y = math_utils.randomInt(state->map->map_height / 2.0, state->map->map_height);
        //   int distance = math_utils.randomInt(400,600);
        //   int start_x = target_x + (int) (distance * state->map->rain_velocity.x);
        //   int start_y = target_y - (int) (distance * state->map->rain_velocity.y);
        //   state->map->rain_targets.push_back({target_x, target_y});
        //   state->map->rain_field.push_back({start_x, start_y});
        // }

        damage_value = math_utils.randomInt(30,40);
        for (BattleCharacter* enemy : magic_targets) {
          enemy->hp -= damage_value;
          if (enemy->hp < 0) {
            enemy->hp = 0;
          }
        }
      } else {
        // Useless
        effects.stop(unique_name + "_magic_effect");
        sound.playSound("toot", 1);
      }
    } else if (current_magic == "Dust") {
      if (state->map->raining) {
        effects.stop(unique_name + "_magic_effect");
        sound.playSound("toot", 1);
      } else {
        state->map->dust_storm = direction;
      }
    }
  }

  if (effects.busy(unique_name + "_magic_effect") && current_magic == "Ice") {
    sound.playSound("ice_shatter_" + to_string(math_utils.randomInt(1, 6)), 1);
  } else if (effects.busy(unique_name + "_magic_effect") && current_magic == "Dust") {
    sound.playSound("dust", 1);
  }

  if (effects.busy(unique_name + "_magic_effect")
    && current_magic == "Dust"
    && damage_value == -1
    && timing.since(unique_name + "_magic_effect") > 1.0) {
    damage_value = 2;
    for (BattleCharacter* enemy : magic_targets) {
      enemy->hp -= damage_value;
      if (enemy->hp < 0) {
        enemy->hp = 0;
      }
      enemy->status_effects["blind"] = true;
    }
  }

  if (effects.finished(unique_name + "_magic_effect")) {
    printf("Now I'm done with magic %s\n", current_magic.c_str());
    if (current_magic == "Ice") {
      state->map->ice_shards = false;
      state->map->rain_velocity = math_utils.rotateVector(0, 1, -1 * state->map->rain_angle);
    } else if (current_magic == "Dust") {
      state->map->dust_storm = 0;
    }
    effects.remove(unique_name + "_magic_effect");
    action_state = "charging";
    ap = 0;

  }
}
