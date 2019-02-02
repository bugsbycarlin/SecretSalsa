/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#pragma once

#include <string>

#include "honey.h"
#include "drawable.h"

using namespace Honey;
using namespace std;

class Character : public Drawable {
 public:
  Character();

  void simpleBounceWalk();

  virtual void walkBehavior();

  virtual void draw(int x, int y);

  ~Character();
};