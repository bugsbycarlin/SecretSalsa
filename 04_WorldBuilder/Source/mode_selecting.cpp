/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "mode_selecting.h"

using namespace std;
using namespace Honey;

ModeSelecting::ModeSelecting(Map* map) {
  this->map = map;
  this->map_width = map->map_width;
  this->map_height = map->map_height;
  printf("Width and height are %d, %d\n", this->map_width, this->map_height);

  if (overlay_surface != NULL) {
    SDL_FreeSurface(overlay_surface);
  }
  overlay_surface = SDL_CreateRGBSurface(0, map_width, map_height, 32, rmask, gmask, bmask, amask);

  pixel_status.resize(map_width);
  for (int k = 0; k < map_width; k++) {
    pixel_status[k].resize(map_height);
  }

  for (int k = 0; k < map_width; k++) {
    for (int l = 0; l < map_height; l++) {
      pixel_status[k][l] = 0;
    }
  }

  camera_x = 0;
  camera_y = 0;

  camera_vx = 0;
  camera_vy = 0;

  eraser = false;

  updateOverlay();
}

void ModeSelecting::initialize() {
}

void ModeSelecting::logic() {
  vector<mouseEvent> mouse_events = input.getMouseEvents();
  for (mouseEvent event : mouse_events) {
    cursor_x = event.x;
    cursor_y = event.y;
    if (event.button == "left") {
      int x = event.x + camera_x;
      int y = event.y + camera_y;
      for (int k = x - stamp_radius; k < x + stamp_radius; k++) {
        for (int l = y - stamp_radius; l < y + stamp_radius; l++) {
          if (k >= 0 && k < map_width && l >= 0 && l < map_height) {
            if (math_utils.distance(x,y,k,l) <= stamp_radius) {
              pixel_status[k][l] = 1;
              if (eraser) pixel_status[k][l] = 0;
            }
          }
        }
      }
      updateOverlay();
    }
  }

  if (input.actionDown("left")) {
    camera_vx -= 1;
  }
  if (input.actionDown("right")) {
    camera_vx += 1;
  }
  if (input.actionDown("up")) {
    camera_vy -= 1;
  }
  if (input.actionDown("down")) {
    camera_vy += 1;
  }
  camera_x += camera_vx;
  camera_y += camera_vy;
  camera_vx *= camera_velocity_decay;
  camera_vy *= camera_velocity_decay;
}

void ModeSelecting::updateOverlay() {
  SDL_LockSurface(overlay_surface);
  unsigned int *overlay_ptr = (unsigned int*)overlay_surface->pixels;

  for (int k = 0; k < map_width; k++) {
    for (int l = 0; l < map_height; l++) {
      int overlay_offset = l * overlay_surface->w;
      Uint32 mask = 0x77111111;
      if (pixel_status[k][l] == 1) {
        mask = 0x00111111;
      } else if (pixel_status[k][l] == 2) {
        mask = 0x44111111;
      }

      overlay_ptr[overlay_offset + k] = mask;
    }
  }

  SDL_UnlockSurface(overlay_surface);

  graphics.removeImage("selection_overlay");
  graphics.addImageFromSurface("selection_overlay", overlay_surface);
}

void ModeSelecting::render() {
  map->draw(-camera_x, -camera_y);

  graphics.drawImage("selection_overlay", -camera_x, -camera_y);
}

ModeSelecting::~ModeSelecting() {
}