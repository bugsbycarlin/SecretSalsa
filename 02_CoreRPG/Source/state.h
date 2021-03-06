/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019

  If coach had just put me in we'da won
*/

#pragma once

#include <array>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include "honey.h"

#include "permanentcharacter.h"
#include "map.h"

using namespace Honey;
using namespace std;

class Mode;

class State {
 public:
  State();

  void store(string label, int value);
  int get(string label);
  bool has(string label);

  void storeString(string label, string value);
  string getString(string label);
  bool hasString(string label);

  void refreshParty();
  void resetMusicEffects();

  ~State();

  stack<Mode*> modes;

  std::string screen_color;
  int screen_width;
  int screen_height;

  Map* map;

  int camera_x;
  int camera_y;
  int camera_target_x;
  int camera_target_y;
  const float camera_blend_factor = 0.2;

  std::map<string, int> values;
  std::map<string, string> string_values;

  std::vector<int> item_counts;
  std::vector<string> item_names;
  
  std::map<string, string> music;

  vector<PermanentCharacter*> party;

  std::map<string, PermanentCharacter*> enemy_templates;
};