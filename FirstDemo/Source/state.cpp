/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "state.h"

using namespace Honey;
using namespace std;

State::State() {
  modes = {};
  values = {};
  string_values = {};
}

void State::store(string label, int value) {
  values[label] = value;
}

int State::get(string label) {
  // For now this will crash if the label doesn't exist
  return values[label];
}

bool State::has(string label) {
  return (values.count(label) == 1);
}

void State::storeString(string label, string value) {
  string_values[label] = value;
}

string State::getString(string label) {
  // For now this will crash if the label doesn't exist
  return string_values[label];
}

bool State::hasString(string label) {
  return (string_values.count(label) == 1);
}

void State::refreshParty() {
  for (PermanentCharacter* character : party) {
    character->hp = character->max_hp;
    character->sp = character->max_sp;
  }

  map->raining = false;
}

void State::resetMusicEffects() {
  values.erase("extra_hp");
  values.erase("tune_bear_berserk");
  values.erase("enemy_items");
  values.erase("all_blind");
  values.erase("slow_battles");
}

State::~State() {
  // To do: delete the stack of modes
  delete map;
}
