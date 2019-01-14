#include <string>

#include "honey.h"

using namespace Honey;

#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>

#include <stdio.h>

#include "honey.h"

using namespace std;
using namespace Honey;
using namespace boost::filesystem;

void initialize();
void render();
void logic();
void cleanup();

void reset();
void setOverlay();
void save();

int mode;
bool quit;

int map_number;
int map_width;
int map_height;

std::vector<std::vector<int>> map_pixel_status;

int camera_x;
int camera_y;
float camera_vx;
float camera_vy;
const float camera_velocity_decay = 0.95;

vector<string> backgrounds = {};

int main(int argc, char* args[]) {
  initialize();

  quit = false;
  // Loop
  while (!quit) {
    logic();
    render();
  }

  cleanup();
}

void initialize() {
  StartHoney("Secret Salsa Map Maker");

  for (directory_iterator itr("Maps/"); itr != directory_iterator{}; ++itr) {
    if (is_regular_file(itr->path())) {
      backgrounds.push_back(itr->path().stem().c_str());
      printf("%s\n", itr->path().filename().c_str());
    }
  }

  graphics.addImages("Maps/", backgrounds);

  map_number = 0;

  camera_x = 0;
  camera_y = 0;

  camera_vx = 0;
  camera_vy = 0;

  reset();
}

void reset() {
  string path = "Maps/" + backgrounds[map_number] + ".png";
  SDL_Surface* image = IMG_Load(path.c_str());
  map_width = image->w;
  map_height = image->h;
  SDL_FreeSurface(image);

  map_pixel_status.resize(map_width);
  for (int k = 0; k < map_width; k++) {
    map_pixel_status[k].resize(map_height);
  }
  // for (int k = 0; k < map_width; k++) {
  //   for (int l = 0; l < map_height; l++) {
  //     if (k % 2 == 0 && l < 100) {
  //       map_pixel_status[k][l] = 1;
  //     }
  //   }
  // }

  setOverlay();
}

void setOverlay() {
  Uint32 rmask, gmask, bmask, amask;
  rmask = 0x00ff0000;
  gmask = 0x0000ff00;
  bmask = 0x000000ff;
  amask = 0xff000000;

  SDL_Surface* overlay_surface = SDL_CreateRGBSurface(0, map_width, map_height, 32, rmask, gmask, bmask, amask);;

  SDL_LockSurface(overlay_surface);
  unsigned int *overlay_ptr = (unsigned int*)overlay_surface->pixels;

  for (int k = 0; k < map_width; k++) {
    for (int l = 0; l < map_height; l++) {
      int overlay_offset = l * overlay_surface->w;
      Uint32 mask = 0x77555555;
      if (map_pixel_status[k][l] != 0) {
        mask = 0x00555555;
      }

      overlay_ptr[overlay_offset + k] = mask;
    }
  }

  SDL_UnlockSurface(overlay_surface);

  graphics.addImageFromSurface("overlay", overlay_surface);

  SDL_FreeSurface(overlay_surface);
}

void logic() {
  input.processInput();

  if (input.keyPressed(",") > 0) {
    map_number -= 1;
    if (map_number < 0) {
      map_number = backgrounds.size() - 1;
    }
    reset();
  }

  if (input.keyPressed(".") > 0) {
    map_number += 1;
    if (map_number >= backgrounds.size()) {
      map_number = 0;
    }
    reset();
  }

  // for (int i = 0; i < 500; i++) {
  //   int x = math_utils.randomInt(0,500);
  //   int y = math_utils.randomInt(0,500);
  //   map_pixel_status[x][y] = 1;
  //   setOverlay();
  //   printf("Loop %d\n", x);
  // }

  if (input.keyDown("left")) {
    camera_vx -= 1;
  }
  if (input.keyDown("right")) {
    camera_vx += 1;
  }
  if (input.keyDown("up")) {
    camera_vy -= 1;
  }
  if (input.keyDown("down")) {
    camera_vy += 1;
  }
  camera_x += camera_vx;
  camera_y += camera_vy;
  camera_vx *= camera_velocity_decay;
  camera_vy *= camera_velocity_decay;

  if (input.keyPressed("quit") > 0) {
    quit = true;
  }

  if (input.threeQuickKey("escape")) {
    quit = true;
  }
}

void render() {
  // Clear the screen to dark grey
  graphics.clearScreen(hot_config.getString("layout", "screen_color"));

  // Switch to 2D drawing mode
  graphics.draw2D();

  graphics.drawImage(backgrounds[map_number], -camera_x, -camera_y);

  graphics.drawImage("overlay", -camera_x, -camera_y);

  // Put everything we've drawn on screen
  graphics.updateDisplay();
}

void cleanup() {

}
