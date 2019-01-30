/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "talkin.h"

using namespace std;
using namespace Honey;

Talkin::Talkin(State* state) {
  this->state = state;
}

void Talkin::initialize() {
  coach_bulldog_x = hot_config.getInt("layout", "coach_bulldog_x");
  coach_bulldog_y = hot_config.getInt("layout", "coach_bulldog_y");

  tune_bear_x = hot_config.getInt("layout", "tune_bear_x");
  tune_bear_y = hot_config.getInt("layout", "tune_bear_y");

  state->camera_x = hot_config.getInt("layout", "conversation_camera_x");;
  state->camera_y = hot_config.getInt("layout", "conversation_camera_y");;

  conversation = new Conversation(state);
  conversation->loadConversations("Scripts/first_demo_conversations.txt");
  printf("Here 1\n");
  conversation->setCurrentConversation("training_introduction");
}

void Talkin::logic() {
  if (input.keyPressed("up") > 0) {
    conversation->up();
  } else if (input.keyPressed("down") > 0) {
    conversation->down();
  } else if (input.keyPressed("a") > 0) {
    conversation->accept();
  }

  if (conversation->finished()) {
    state->modes.push(new Walkin(state));
    state->modes.top()->initialize();
  }
}

void Talkin::render() {
  state->map->draw(-state->camera_x, -state->camera_y);

  graphics.drawImage(
    "coach_bulldog",
    coach_bulldog_x - state->camera_x,
    coach_bulldog_y - state->camera_y,
    true, 0, 1
  );

  graphics.drawImage(
    "tune_bear",
    tune_bear_x - state->camera_x,
    tune_bear_y - state->camera_y,
    true, 0, 1
  );

  conversation->draw();
}

Talkin::~Talkin() {
  delete conversation;
}