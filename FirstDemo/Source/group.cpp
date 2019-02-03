/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "group.h"

using namespace std;
using namespace Honey;

Group::Group(State* state) {
  this->state = state;
  characters = {};
  walk_trailing_distance = hot_config.getInt("animation", "walk_trailing_distance");
}

void Group::add(Character* character) {
  characters.push_back(character);
}

void Group::walk(float ax, float ay) {
  characters[0]->walkBehavior(ax, ay);
  followLeader();
}

void Group::seek(int x, int y) {
  characters[0]->seekBehavior(x, y);
  followLeader();
}

void Group::followLeader() {
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

void Group::draw() {
  draw_order_characters = characters;
  std::sort(std::begin(draw_order_characters), std::end(draw_order_characters), [](Character* a, Character* b) {return a->y > b->y;});
  for (int i = draw_order_characters.size() - 1; i >= 0; i--) {
    draw_order_characters[i]->draw();
  }
}

Group::~Group() {
  for (int i = 0; i < characters.size(); i++) {
    delete characters[i];
  }
}