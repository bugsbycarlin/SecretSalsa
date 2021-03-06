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

  timing.remove("conversation_fade_out");
  conversation = new Conversation(state);
  conversation->loadConversations("Scripts/first_demo_conversations.txt");
  printf("Here 1\n");
  conversation->setCurrentConversation("training_introduction");
  sound.stopMusic();
  sound.setMusicVolume(hot_config.getFloat("music", "music_volume"));

  timing.mark("talking_fade_in");
}

void Talkin::logic() {
  if (input.keyPressed("up") > 0) {
    conversation->up();
  } else if (input.keyPressed("down") > 0) {
    conversation->down();
  } else if (input.keyPressed("a") > 0) {
    conversation->accept();
  }

  if (conversation->finished && !timing.check("conversation_fade_out")) {
    timing.mark("conversation_fade_out");
    if (conversation->finish_value == "fade") {
      if (!timing.check("end_state_pause")) {
        timing.mark("end_state_pause");
      }
    }
  }

  if (timing.check("end_state_pause") && timing.since("end_state_pause") > 1.0) {
    timing.remove("end_state_pause");
    if (conversation->getCurrentConversation() == "training_failure") {
      sound.playMusic("game_over", 1);
    } else if (conversation->getCurrentConversation() == "training_success") {
      sound.playMusic("win_music", 1);
    }
  }

  if (timing.since("conversation_fade_out") > 0.5 && conversation->finished) {
    if (conversation->finish_value == "walking") {
      state->refreshParty();
      state->resetMusicEffects();
      state->store("laps", 0);
      state->modes.push(new Walkin(state));
      state->modes.top()->initialize();
    }
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
    true, 0, -1, 1, 1
  );

  state->map->overlayer(-state->camera_x, -state->camera_y);

  conversation->draw();

  if (conversation->finished && conversation->finish_value == "walking") {
    //printf("I AM HERE %0.2f, %0.2f, %0.2f\n", timing.since("conversation_fade_out"), 1280 * (1.0 - timing.since("conversation_fade_out"), -720.0 + 720.0 * timing.since("conversation_fade_out"));
    int x = (int) 1280 * (1.0 - 2 * timing.since("conversation_fade_out"));
    int y = (int) -720.0 + 720.0 * 2 * timing.since("conversation_fade_out");
    graphics.drawImage("black_screen", x, y);
  } else if (conversation->finished && conversation->finish_value == "fade") {
    graphics.setColor("#FFFFFF", timing.since("conversation_fade_out"));
    graphics.drawImage("black_screen", 0, 0);
  }

  if (timing.since("talking_fade_in") <= 0.5) {
    int x = (int) -1280 * 2 * timing.since("talking_fade_in");
    int y = (int) 720.0 * 2 * timing.since("talking_fade_in");
    graphics.drawImage("black_screen", x, y);
  }
}

Talkin::~Talkin() {
  delete conversation;
}