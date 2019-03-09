/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "honey.h"

#include <string>
#include <array>

#include "worldbuilder.h"

using namespace Honey;
using namespace std;

int main(int argc, char* args[]) {
  StartHoney("Secret Salsa WorldBuilder");

  unique_ptr<Screen> worldbuilder(new WorldBuilder());

  screenmanager.addScreen("worldbuilder", worldbuilder);
  screenmanager.setCurrentScreen("worldbuilder");
  screenmanager.initialize();

  graphics.draw2D();

  while (!screenmanager.getQuit()) {
    input.processInput();

    screenmanager.loop();

    // Put everything we've drawn on screen
    graphics.updateDisplay();
  }

  worldbuilder.reset();
}
