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
#include "permanentcharacter.h"

using namespace Honey;
using namespace std;

class Character {
 public:
  Character(State* state);

  virtual void addAnimation(string name, vector<string> animation);
  virtual void setAnimation(string animation);
  virtual void setFrame(int frame);

  void addBounceAnimation();
  void walkAnimation();

  virtual void draw();

  void cloneFromPermanentCharacter(PermanentCharacter* character);

  ~Character();

  State* state;
  PermanentCharacter* permanent_character;

  string unique_name;

  string name;
  string display_name;

  map<string, vector<string>> animations;
  string current_animation;
  int current_frame;

  int x;
  int y;

  int direction; // -1 is left, +1 is right

  // Display parameters
  int margin_x;
  int margin_y;

  // Animation parameters
  int bounce_y;
  int rotation;
  float walk_animation_speed;

  int hp;
  int max_hp;
  int sp;
  int max_sp;
  float ap_rate;

  float dodge;
  int defense;
  int attack_min;
  int attack_max;

  string item;

  string skill;
  vector<string> skill_list;

 protected:
  static int unique_count;
};