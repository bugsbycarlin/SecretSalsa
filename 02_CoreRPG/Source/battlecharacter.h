/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <map>
#include <string>
#include <vector>

#include "honey.h"
#include "character.h"

using namespace Honey;
using namespace std;

class BattleCharacter : public Character {
 public:
  BattleCharacter(State* state);

  ~BattleCharacter();

  void walkToStartingPosition();
  void startAutomaticBattle(vector<BattleCharacter*> opposing_party);
  void startAttack();
  void continueAttack();
  void startWalk();

  void draw();
  void drawPrepMode();
  void drawActiveMode();

  void cloneFromCharacter(Character* character);

  const float prep_walk_velocity = 5.0;

  float ap;
  const float max_ap = 100;

  int battle_direction;

  int frame_counter;

  int battle_x;
  int battle_y;
  int battle_home_x;
  int battle_home_y;

  int battle_row;
  int battle_column;

  int damage_value;

  float attack_jump_time;
  float attack_hold_time;

  string action_state; // charging, ready, waiting, choosing, acting, skill, walking, ko

  bool player_character;

  BattleCharacter* target;

  map<string, bool> status_effects;


  // Skill specific methods and parameters
  void startMagic();
  void continueMagic();

  string current_magic;
  vector<BattleCharacter*> magic_targets;

  float magic_incantation_time;
  float magic_effect_time;

  bool attack_using_skill;

};