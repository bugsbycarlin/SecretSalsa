/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "intro.h"

using namespace std;
using namespace Honey;

Intro::Intro(State* state) {
  this->state = state;
}

void Intro::initialize() {
  timing.mark("intro_fade_in");
}

void Intro::logic() {
  if (input.actionPressed("accept") > 0 && !timing.check("intro_fade_out")) {
    sound.playSound("accept_sound", 1);
    timing.mark("intro_fade_out");
  }

  if (timing.check("intro_fade_out") && timing.since("intro_fade_out") > 0.55) {
    state->modes.push(new Talkin(state));
    state->modes.top()->initialize();
  }
}

void Intro::render() {
  graphics.clearScreen("#dedede");

  graphics.setColor("#FFFFFF", 1.0);
  graphics.drawImage("intro_screen", 0, 0);

  float opacity = 1.0 - timing.since("intro_fade_in") / 2.0;
  if (opacity < 0.0) opacity = 0.0;
  graphics.setColor("#FFFFFF", opacity);
  graphics.drawImage("black_screen", 0, 0);

  if (timing.check("intro_fade_out")) {
    graphics.setColor("#FFFFFF", 1.0);
    int x = (int) 1280 * (1.0 - 2 * timing.since("intro_fade_out"));
    int y = (int) -720.0 + 720.0 * 2 * timing.since("intro_fade_out");
    if (timing.since("intro_fade_out") > 0.5) {
      x = 0;
      y = 0;
    }
    graphics.drawImage("black_screen", x, y);
  } 
}

Intro::~Intro() {
}