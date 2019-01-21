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
}

void State::store(string label, int value) {
  values[label] = value;
}

int State::get(string label) {
  // For now this will crash if the label doesn't exist
  return values[label];
}

State::~State() {
  // To do: delete the stack of modes
  delete map;
}
