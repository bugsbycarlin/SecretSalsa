/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "drawable.h"

using namespace Honey;
using namespace std;

Drawable::Drawable() {
  this->label = "";
  this->x = 0;
  this->y = 0;
}

Drawable::Drawable(string label, int x, int y) {
  this->label = label;
  this->x = x;
  this->y = y;
}

void Drawable::draw(int camera_x, int camera_y) {
  graphics.drawImage(label, x - camera_x, y - camera_y, true, 0, 1);
}

Drawable::~Drawable() {
}
