/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "permanentcharacter.h"

using namespace std;
using namespace Honey;

int PermanentCharacter::unique_count = 0;

PermanentCharacter::PermanentCharacter(State* state) {
  this->state = state;

  unique_name = "permanent_character_" + to_string(unique_count);
  unique_count++;
}

void PermanentCharacter::loadFromConfig(string name) {
  this->name = name;
  display_name = hot_config.getString("game", name + "_display_name");

  max_hp = hot_config.getInt("game", name + "_hp");
  hp = max_hp;
  max_sp = hot_config.getInt("game", name + "_sp");
  sp = max_sp;
  ap_rate = hot_config.getFloat("game", name + "_ap_rate");

  defense = hot_config.getInt("game", name + "_defense");
  attack_min = hot_config.getInt("game", name + "_attack_min");
  attack_max = hot_config.getInt("game", name + "_attack_max");
  dodge = hot_config.getFloat("game", name + "_dodge");

  skill = hot_config.getString("game", name + "_skill");

  string skill_list_string = hot_config.getString("game", name + "_skill_list");
  boost::split(skill_list, skill_list_string, boost::is_any_of(","), boost::token_compress_on);
}

PermanentCharacter::~PermanentCharacter() {
}