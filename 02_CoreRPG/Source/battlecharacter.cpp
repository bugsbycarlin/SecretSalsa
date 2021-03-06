/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "battlecharacter.h"

using namespace std;
using namespace Honey;

BattleCharacter::BattleCharacter(State* state) : Character(state) {
  action_state = "charging";
  target = NULL;
  damage_value = 0;

  attack_jump_time = hot_config.getFloat("game", "default_battle_jump_time");
  attack_hold_time = hot_config.getFloat("game", "default_battle_hold_time");

  magic_incantation_time = hot_config.getFloat("game", "default_magic_incantation_time");
  magic_effect_time = hot_config.getFloat("game", "default_magic_effect_time");

  status_effects = {};
  status_effects["blind"] = false;
  frame_counter = 0;
}

void BattleCharacter::walkToStartingPosition() {
  if (battle_x < battle_home_x) {
    battle_x += prep_walk_velocity;
  } else if (battle_x > battle_home_x) {
    battle_x -= prep_walk_velocity;
  }
  if (abs(battle_home_x - battle_x) < prep_walk_velocity) battle_x = battle_home_x;
  if (battle_y < battle_home_y) {
    battle_y += prep_walk_velocity;
  } else if (battle_y > battle_home_y) {
    battle_y -= prep_walk_velocity;
  }
  if (abs(battle_home_y - battle_y) < prep_walk_velocity) battle_y = battle_home_y;
}

void BattleCharacter::startAutomaticBattle(vector<BattleCharacter*> opposing_party) {
  int count = 0;
  target = NULL;
  while(target == NULL || target->hp <= 0) {
    target = opposing_party[math_utils.randomInt(0, opposing_party.size())];
    count++;
    if (count > 200) {
      printf("WTF, can't find nonzero hp target\n");
      break;
    }
  }
  int row = target->battle_row;
  int col = target->battle_column;
  // bug if the character is facing backwards but it's not worth handling now
  for (BattleCharacter* character : opposing_party) {
    if (character->hp > 0 && character->battle_row == row && 
      ((character->direction == -1 && col == 1 && character->battle_column == 0)
        || (character->direction == 1 && col == 0 && character->battle_column == 1))) {
      // must target a closer character
      target = character;
    }
  }
  if (target->hp > 0) {
    startAttack();
  } else {
    action_state = "charging";
    ap = 0;
  }
}

void BattleCharacter::startWalk() {
  action_state = "walking";
}

void BattleCharacter::startAttack() {
  action_state = "acting";
  effects.makeTween(unique_name + "_attack_move_x", battle_home_x, target->battle_home_x + target->direction * 70, attack_jump_time);
  effects.makeTween(unique_name + "_attack_jump_y", 0, 200, attack_jump_time);
  effects.makeTween(unique_name + "_attack_move_y", battle_home_y, target->battle_home_y, attack_jump_time);
  effects.start(unique_name + "_attack_move_x");
  effects.start(unique_name + "_attack_jump_y");
  effects.start(unique_name + "_attack_move_y");
}

void BattleCharacter::continueAttack() {
  if (effects.busy(unique_name + "_attack_move_x")) {
    battle_x = effects.tween(unique_name + "_attack_move_x", effects.LINEAR);
    battle_y = effects.tween(unique_name + "_attack_move_y", effects.LINEAR);
    int jump_y = effects.tween(unique_name + "_attack_jump_y", effects.CUBIC);
    if (jump_y >= 100) {
      jump_y = 200 - jump_y;
    }
    battle_y -= jump_y;
  }

  if (effects.busy(unique_name + "_attack_return_x")) {
    battle_x = effects.tween(unique_name + "_attack_return_x", effects.LINEAR);
    battle_y = effects.tween(unique_name + "_attack_return_y", effects.LINEAR);
    int jump_y = effects.tween(unique_name + "_attack_jump_y", effects.CUBIC);
    if (jump_y >= 100) {
      jump_y = 200 - jump_y;
    }
    battle_y -= jump_y;
  }

  if (effects.finished(unique_name + "_attack_move_x")) {
    //printf("Switching from attack_move to attack_hold\n");
    effects.remove(unique_name + "_attack_move_x");
    effects.remove(unique_name + "_attack_jump_y");
    effects.remove(unique_name + "_attack_move_y");
    effects.makeTween(unique_name + "_attack_hold_move", 0, 0, attack_hold_time);
    effects.start(unique_name + "_attack_hold_move");

    // TODO dodge here
    int miss_check = status_effects["blind"] ? 650 : 0;
    if (target->battle_x == target->battle_home_x
      && target->battle_y == target->battle_home_y
      && math_utils.randomInt(0, 1000) > (target->dodge * 1000.0 + miss_check)) {
      if (skill == "Forage" && attack_using_skill == true) {
        if (target->item != "None") {
          for (int i = 0; i < state->item_names.size(); i++) {
            if (target->item == state->item_names[i]) {
              state->item_counts[i] += 1;
            }
          }
          sound.playSound(name + "_attack", 1);
          effects.makeTween("battle_item_use", 1.0, 0.0, 1.0);
          effects.start("battle_item_use");
          // very bad design
          state->storeString("battle_item_use", target->item);
          target->item = "None";
        } else {
          sound.playSound("miss", 1);
        }
      } else {
        sound.playSound(name + "_attack", 1);
        damage_value = math_utils.randomInt(attack_min, attack_max) - target->defense;
        if (damage_value < 1) damage_value = 1;
        if (state->get("tune_bear_berserk") && name == "tune_bear") {
          if (math_utils.randomInt(0, 100) > 40) {
            damage_value *= 2;
          }
        }
        target->hp -= damage_value;
        if (target->hp < 0) {
          target->hp = 0;
        }
        timing.mark(target->unique_name + "_hurt");
      }
    } else {
      // MISS!
      damage_value = -1;
      sound.playSound("miss", 1);
    }
  }

  if (effects.finished(unique_name + "_attack_hold_move")) {
    //printf("Switching from attack_hold_move to attack_return\n");
    effects.remove(unique_name + "_attack_hold_move");
    effects.makeTween(unique_name + "_attack_return_x", battle_x, battle_home_x, attack_jump_time);
    effects.makeTween(unique_name + "_attack_jump_y", 0, 200, attack_jump_time);
    effects.makeTween(unique_name + "_attack_return_y", battle_y, battle_home_y, attack_jump_time);
    effects.start(unique_name + "_attack_return_x");
    effects.start(unique_name + "_attack_jump_y");
    effects.start(unique_name + "_attack_return_y");
  } else {
    effects.tween(unique_name + "_attack_hold_move", effects.LINEAR); // this is a hack to check it; if unchecked, unfinished.
  }


  if (effects.finished(unique_name + "_attack_return_x")) {
    state->storeString("battle_item_use", "");
    attack_using_skill = false;
    effects.remove(unique_name + "_attack_return_x");
    effects.remove(unique_name + "_attack_jump_y");
    effects.remove(unique_name + "_attack_return_y");
    battle_x = battle_home_x;
    battle_y = battle_home_y;
    action_state = "charging";
    ap = 0;
  }
}

void BattleCharacter::draw() {

}

void BattleCharacter::drawPrepMode() {
  frame_counter += 1;
  int bounce_y = effects.tween("simple_bounce_walk_" + unique_name, effects.CUBIC);
  if (bounce_y > hot_config.getInt("animation", "walk_bounce_height")) {
    bounce_y = 2 * hot_config.getInt("animation", "walk_bounce_height") - bounce_y;
  }
  int rotation = effects.oscillation("simple_bounce_lean_" + unique_name);
  walkAnimation();

  if (effects.finished("simple_bounce_walk_" + unique_name)) {
    effects.start("simple_bounce_walk_" + unique_name);
  }

  graphics.setColor("#FFFFFF", 1.0);
  graphics.drawImage(
    animations[current_animation][current_frame],
    battle_x + margin_x,
    battle_y + margin_y - bounce_y,
    true, rotation, direction, 1, 1
  );
}

void BattleCharacter::drawActiveMode() {
  frame_counter += 1;
  bool flip = false;
  setAnimation("static");
  if (animations.count("attacking") == 1 &&
    (effects.busy(unique_name + "_attack_move_x")
    || effects.busy(unique_name + "_attack_hold_move")
    || effects.busy(unique_name + "_attack_return_x"))) {
    setAnimation("attacking");
  } else if (action_state == "walking") {
    drawPrepMode();
    return;
  } else if (action_state != "acting" && hp <= 0) {
    if (animations.count("ko") == 1) {
      setAnimation("ko");
    } else {
      flip = true;
    }
  } else if (timing.check(unique_name + "_hurt") 
    && timing.since(unique_name + "_hurt") < hot_config.getFloat("animation", "hurt_time")
    && animations.count("hurt") == 1) {
    setAnimation("hurt");
    setFrame(0);
  } else if (action_state == "skill" && effects.busy(unique_name + "_magic_incantation") && frame_counter % 10 == 0) {
    if (animations.count("casting") == 1 && animations["casting"].size() > 1) {
      current_frame += 1;
      if (current_frame >= animations["casting"].size()) current_frame = 0;
      setAnimation("casting");
      setFrame(current_frame);
    }
  } else {
    setAnimation("static");
    setFrame(0);
  }

  if (current_frame > animations[current_animation].size() - 1) {
    current_frame = current_frame % animations[current_animation].size();
  }

  int hurt_knockback = 0;
  if (timing.check(unique_name + "_hurt") 
    && timing.since(unique_name + "_hurt") >= hot_config.getFloat("animation", "hurt_time")) {
    timing.remove(unique_name + "_hurt");
  } else if (timing.check(unique_name + "_hurt") 
    && timing.since(unique_name + "_hurt") < hot_config.getFloat("animation", "hurt_time")) {
    hurt_knockback = hot_config.getInt("animation", "hurt_knockback");
  }

  int bounce_y = 0;
  if (action_state == "choosing") {
    bounce_y = effects.tween("simple_bounce_walk_" + unique_name, effects.CUBIC);
    if (bounce_y > hot_config.getInt("animation", "walk_bounce_height")) {
      bounce_y = 2 * hot_config.getInt("animation", "walk_bounce_height") - bounce_y;
    }

    if (effects.finished("simple_bounce_walk_" + unique_name)) {
      effects.start("simple_bounce_walk_" + unique_name);
    }
  }

  graphics.setColor("#FFFFFF", 1.0);
  if (state->get("tune_bear_berserk") && name == "tune_bear") {
    graphics.setColor("#FF0000", 1.0);
  }
  graphics.drawImage(
    animations[current_animation][current_frame],
    battle_x + margin_x - direction * hurt_knockback,
    battle_y + margin_y - bounce_y,
    true, 0, direction, flip ? -1 : 1, 1
  );

  // effects.shake(unique_name + "_magic_incantation")
  if (action_state == "skill" && effects.busy(unique_name + "_magic_incantation")) {
    graphics.drawImage(
    "spellbook",
    battle_x + hot_config.getInt("game", "spellbook_margin_x"),
    battle_y + hot_config.getInt("game", "spellbook_margin_y"),
    true, 0, direction, 1, 1
  );
  }

  if (effects.busy(unique_name + "_attack_hold_move")) {
    graphics.drawImage(
      "attack_slash_thick",
      target->battle_x,
      target->battle_y + target->margin_y,
      true, 0, target->direction, 1, 1
    );
  }

  if (status_effects["blind"] && hp > 0 && action_state != "acting") {
    graphics.drawImage(
      "glasses",
      battle_x - 5,
      battle_y + margin_y - bounce_y - 28,
      true, 0, direction, 1, 1
    );
  }
}

void BattleCharacter::cloneFromCharacter(Character* character) {
  name = character->name;
  display_name = character->display_name;

  animations = character->animations;
  current_animation = character->current_animation;
  current_frame = character->current_frame;

  x = character->x;
  y = character->y;

  direction = character->direction;

  margin_x = character->margin_x;
  margin_y = character->margin_y;

  bounce_y = character->bounce_y;
  rotation = character->rotation;
  walk_animation_speed = character->walk_animation_speed;

  hp = character->hp;
  max_hp = character->max_hp;
  sp = character->sp;
  max_sp = character->max_sp;
  ap_rate = character->ap_rate;

  attack_min = character->attack_min;
  attack_max = character->attack_max;
  defense = character->defense;
  dodge = character->dodge;

  item = character->item;

  skill = character->skill;
  skill_list = character->skill_list;

  permanent_character = character->permanent_character;
}

BattleCharacter::~BattleCharacter() {
}
