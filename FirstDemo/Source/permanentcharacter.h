/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <map>
#include <string>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "honey.h"
#include "state.h"

using namespace Honey;
using namespace std;

class PermanentCharacter {
 public:
  PermanentCharacter(State* state);

  void loadFromConfig(string name);

  ~PermanentCharacter();

  State* state;

  string unique_name;

  string name;
  string display_name;

  int hp;
  int max_hp;
  float ap_rate;

  float dodge;
  int defense;
  int attack_min;
  int attack_max;

  string skill;
  vector<string> skill_list;

 protected:
  static int unique_count;
};