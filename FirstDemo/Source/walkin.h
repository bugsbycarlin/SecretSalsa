/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>

#include "honey.h"
#include "character.h"
#include "group.h"
#include "mode.h"
#include "battlin.h"

using namespace Honey;
using namespace std;

class Walkin : public Mode {
 public:
  Walkin(State* state);

  void initialize();
  void logic();
  void render();

  void initializeCharacters();
  void addBaddieGroup();

  void movementLogic();
  void animationLogic();
  void cameraLogic();
  void gameLogic();

  bool checkBox(int x, int y, box b);

  ~Walkin();

  int coach_bulldog_x;
  int coach_bulldog_y;

  Group* party_group;
  vector<Group*> baddie_groups;
  vector<box*> baddie_seek_points;
  vector<int> baddie_seek_numbers;

  // Textbox* hp_box;
  // Textbox* laps_box;

  vector<box> lap_zones;
  vector<box> lap_seek_points;
  int lap_zone_counter;
  Menu* lap_count_box;

  int battle_trigger_distance;
};