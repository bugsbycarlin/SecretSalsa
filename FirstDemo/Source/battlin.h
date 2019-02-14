/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <queue>
#include <string>

#include "honey.h"
#include "mode.h"
#include "walkingcharacter.h"
#include "battlecharacter.h"
#include "talkin.h"

using namespace Honey;
using namespace std;

class Talkin;

class Battlin : public Mode {
 public:
  Battlin(State* state, vector<WalkingCharacter*> player_party, vector<WalkingCharacter*> enemy_party);

  void initialize();
  void logic();
  void render();

  void initializeCharacters();
  void initializeMenus();

  void everybodyGetInPlace();
  void chargeGauges();
  void handleSelection();

  void cleanUpEffects();

  void drawInfoCard();
  void drawSelection();

  ~Battlin();

  Textbox* hp_box;

  float ap_delay;

  string mode; // prep, action, finished

  int good_direction;

  vector<WalkingCharacter*> root_player_party;
  vector<WalkingCharacter*> root_enemy_party;

  vector<BattleCharacter*> player_party;
  vector<BattleCharacter*> enemy_party;
  vector<BattleCharacter*> everyone;

  queue<BattleCharacter*> action_queue;
  queue<BattleCharacter*> enemy_queue;

  BattleCharacter* selection_character;
  int selection_state = 1;
  int selection_1;
  int selection_2;
  int selection_3;
  int selection_row;
  int selection_column;

  BattleCharacter* item_character;
  string item_name;

  const int selection_1_max = 2;

  // TODO Destroy all of these in the destructor
  Menu* header;
  Menu* info_card;
  Menu* selection_card_1_header;
  Menu* selection_card_2_header;
  Menu* selection_card_1;
  Menu* selection_card_2;

  Textbox* damage_text;

  Sprite* black_screen;

  int battle_frame_counter;

 private:
  string printLine(vector<string> information, vector<int> widths);
};