/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "drawable.h"

using namespace Honey;
using namespace std;

// Drawable::Drawable() {
//   animations = {};
//   setAnimation("default");
//   setFrame(0);
//   this->x = 0;
//   this->y = 0;
// }

Drawable::Drawable(string label, int x, int y) {
  animations = {};
  addAnimation("default", {label});
  setAnimation("default");
  setFrame(0);
  this->x = x;
  this->y = y;
}

void Drawable::addAnimation(string animation, vector<string> animation_frames) {
  animations[animation] = animation_frames;
}

void Drawable::setAnimation(string animation) {
  current_animation = animation;
}

void Drawable::setFrame(int frame) {
  current_frame = frame;
}

void Drawable::draw(int camera_x, int camera_y) {
  //printf("Current picture is %s\n", animations[current_animation][current_frame].c_str());
  graphics.drawImage(animations[current_animation][current_frame], x - camera_x, y - camera_y, true, 0, 1);
}

Drawable::~Drawable() {
}
