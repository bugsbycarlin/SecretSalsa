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
}

State::~State() {
  // To do: delete the stack of modes
  delete map;
}
