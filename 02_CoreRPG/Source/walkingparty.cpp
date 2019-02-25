/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "walkingparty.h"

using namespace std;
using namespace Honey;

WalkingParty::WalkingParty(State* state) {
  this->state = state;
  characters = {};
  walk_trailing_distance = hot_config.getInt("animation", "walk_trailing_distance");
}

void WalkingParty::add(WalkingCharacter* character) {
  characters.push_back(character);
}

void WalkingParty::walk(float ax, float ay) {
  characters[0]->walkBehavior(ax, ay);
  followLeader();
}

void WalkingParty::seek(int x, int y) {
  characters[0]->seekBehavior(x, y);
  followLeader();
}

void WalkingParty::followLeader() {
  for (int i = 1; i < characters.size(); i++) {
    if (characters[0]->position_history.size() > i * walk_trailing_distance) {
      characters[i]->followBehavior(
        characters[0]->position_history[i * walk_trailing_distance].x,
        characters[0]->position_history[i * walk_trailing_distance].y);
    } else if (characters[0]->position_history.size() > 0) {
      characters[i]->followBehavior(
        characters[0]->position_history[characters[0]->position_history.size() - 1].x,
        characters[0]->position_history[characters[0]->position_history.size() - 1].y);
    }
  }
}

void WalkingParty::draw() {
  draw_order_characters = characters;
  std::sort(std::begin(draw_order_characters), std::end(draw_order_characters), [](WalkingCharacter* a, WalkingCharacter* b) {return a->y > b->y;});
  for (int i = draw_order_characters.size() - 1; i >= 0; i--) {
    draw_order_characters[i]->draw();
  }
}

WalkingParty::~WalkingParty() {
  for (int i = 0; i < characters.size(); i++) {
    delete characters[i];
  }
}