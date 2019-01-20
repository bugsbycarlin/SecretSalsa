/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "honey.h"

#include <string>
#include <array>

#include "game.h"

using namespace Honey;
using namespace std;

int main(int argc, char* args[]) {
  StartHoney("Secret Salsa First Demo");

  unique_ptr<Screen> game(new Game());

  screenmanager.addScreen("game", game);
  screenmanager.setCurrentScreen("game");
  screenmanager.initialize();

  while (!screenmanager.getQuit()) {
    input.processInput();

    screenmanager.loop();

    // Put everything we've drawn on screen
    graphics.updateDisplay();
  }

  game.reset();
}
