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

  int defense;
  int attack_min;
  int attack_max;

 protected:
  static int unique_count;
};