/*
  Secret Salsa First Demo
  Matthew Carlin
  Copyright 2019
*/

#include <string>

#include "honey.h"

#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace Honey;
using namespace boost::filesystem;

void initialize();
void render();
void logic();
void cleanup();

void reset();
void makeWindyGrass();

void selectGrassModeLogic();
void displayModeLogic();

//
// Params
//

// universal params
string mode;
bool quit;
int camera_x;
int camera_y;
float camera_vx;
float camera_vy;
const float camera_velocity_decay = 0.95;
SDL_Surface* old_surface;

// selection mode params
const int max_stamp_radius = 100;
const int default_stamp_radius = 20;
int stamp_radius;
bool eraser;
int cursor_x;
int cursor_y;
SDL_Surface* overlay_surface;

// displayMode params
int frame_count = 0;

// grass params
vector<float> wind_frames; // {-0.5, -0.25, 0, 0.25, 0.5, 0.625, 0.75, 0.875, 1.0};
vector<SDL_Surface*> windy_surfaces;

vector<string> backgrounds = {};

int map_number;
int map_width;
int map_height;
std::vector<std::vector<int>> map_pixel_status;
std::vector<std::vector<int>> save_pixels;

//
// Methods
//

void db(int x) {
  printf("Here %d\n", x);
}

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

  backgrounds = {"running_track"};

  graphics.addImages("Maps/", backgrounds);

  graphics.addImage("cursor", "Art/cursor.png");
  graphics.addImage("eraser", "Art/eraser.png");

  map_number = 0;

  camera_x = hot_config.getInt("layout", "display_x");
  camera_y = hot_config.getInt("layout", "display_y");

  camera_vx = 0;
  camera_vy = 0;

  cursor_x = -500;
  cursor_y = -500;

  stamp_radius = default_stamp_radius;

  eraser = false;

  wind_frames = {-0.5, -0.25, 0, 0.25, 0.5, 0.625, 0.75, 0.875, 1.0};

  reset();

  timing.mark("animation_delay");

  mode = "displayMode";
}

void reset() {
  if (old_surface != NULL) {
    SDL_FreeSurface(old_surface);
  }
  string path = "Maps/" + backgrounds[map_number] + ".png";
  old_surface = IMG_Load(path.c_str());
  map_width = old_surface->w;
  map_height = old_surface->h;

  map_pixel_status.resize(map_width);
  for (int k = 0; k < map_width; k++) {
    map_pixel_status[k].resize(map_height);
  }

  for (int k = 0; k < map_width; k++) {
    for (int l = 0; l < map_height; l++) {
      map_pixel_status[k][l] = 0;
    }
  }

  if (overlay_surface != NULL) {
    SDL_FreeSurface(overlay_surface);
  }

  Uint32 rmask, gmask, bmask, amask;
  rmask = 0x00ff0000;
  gmask = 0x0000ff00;
  bmask = 0x000000ff;
  amask = 0xff000000;

  overlay_surface = SDL_CreateRGBSurface(0, map_width, map_height, 32, rmask, gmask, bmask, amask);

  for (int i = 0; i < wind_frames.size(); i++) {
    SDL_Surface* x = SDL_CreateRGBSurface(0, map_width, map_height, 32, rmask, gmask, bmask, amask);
    windy_surfaces.push_back(x);
  }

  makeWindyGrass();

  //SDL_FreeSurface(old_surface);
}

void makeWindyGrass() {
  vector<unsigned int *>windy_surface_ptrs;
  for (int i = 0; i < wind_frames.size(); i++) {
    SDL_LockSurface(windy_surfaces[i]);
    windy_surface_ptrs.push_back((unsigned int *)windy_surfaces[i]->pixels);
  }

  unsigned int *old_surface_ptr = (unsigned int*)old_surface->pixels;

  // Copy the old surface to the new
  for (int k = 0; k < map_width; k++) {
    for (int l = 0; l < map_height; l++) {
      int offset = l * old_surface->w;
      for (int i = 0; i < wind_frames.size(); i++) {
        windy_surface_ptrs[i][offset + k] = old_surface_ptr[offset + k];
      }
    }
  }

  timing.setOverrideTime(0.0);

  // Set up the parameters
  int wind_strength_standard = hot_config.getInt("grass", "wind_strength");
  int brush_stroke_count = hot_config.getInt("grass", "brush_stroke_count");
  int brush_stroke_length_standard = hot_config.getInt("grass", "brush_stroke_length");
  int width = hot_config.getInt("grass", "brush_width");
  int height = hot_config.getInt("grass", "brush_height");
  int density = hot_config.getInt("grass", "brush_density");
  float opacity = hot_config.getFloat("grass", "brush_opacity");
  float blend_factor = hot_config.getFloat("grass", "brush_blend_factor");

  // Grab a series of points. We're going to sort these in Y order so they draw correctly.
  vector<position> root_positions;
  for (int c = 0; c < brush_stroke_count; c++) {
    root_positions.push_back({
      math_utils.randomInt(0, old_surface->w),
      math_utils.randomInt(hot_config.getInt("grass", "grass_y"), 1161)
    });
  }
  sort(root_positions.begin(), root_positions.end());
  // for (int i = 0; i < 10; i++) {
  //   printf("%d,%d\n", root_positions[i].x, root_positions[i].y);
  // }

  // Do a series of brush strokes
  for (int c = 0; c < brush_stroke_count; c++) {
    std::vector<std::vector<int>> brush;

    // Make a brush stamp from random values according to the configured width, height, and density
    brush.resize(width);
    for (int j = 0; j < width; j++) {
      brush[j].resize(height);
    }
    for (int j = 0; j < density; j++) {
      brush[math_utils.randomInt(0, width)][math_utils.randomInt(0, height)] = 1;
    }


    // Vary the length a little bit from configured value to make it more natural
    int brush_stroke_length = brush_stroke_length_standard + math_utils.randomInt(-1 * (int) (brush_stroke_length_standard / 3.0), (int) (brush_stroke_length_standard / 3.0));

    // Save the root color and x,y location of the stroke
    int root_x = root_positions[c].x;
    int root_y = root_positions[c].y;
    int root_color_address = root_y * old_surface->w + root_x;
    Uint8 root_r, root_b, root_g, root_a;
    SDL_GetRGBA(old_surface_ptr[root_color_address], old_surface->format, &root_r, &root_g, &root_b, &root_a);

    // For each desired frame of animated grass, perform the stroke
    for (int i = 0; i < wind_frames.size(); i++) {
      // Set the wind strength as configured strength * frame fraction (eg some frames are a little and some are full)
      // Also vary this a little bit to make it more natural
      int wind_frame_root = wind_frames[i] * wind_strength_standard;
      int wind_strength = math_utils.randomInt(-3,3);
      if (abs(wind_frame_root) > 0) {
        wind_strength = wind_frame_root + math_utils.randomInt(-1 * (int) (abs(wind_frame_root) / 4.0), (int) (abs(wind_frame_root) / 4.0));
      }
      // printf("Frame %d, strength %d\n", i, wind_strength);
      
      // Control the timing and make a series of effects; we'll force timing from 0 to 1 and
      // use the effects to interpolate the location
      timing.setOverrideTime(0.0);
      effects.makeTween("wind", 0, wind_strength, 10.0); // rampup
      effects.start("wind");
      effects.makeTween("stroke", 0, brush_stroke_length - abs(wind_strength) / 2.0, 10.0); // linear
      effects.start("stroke");

      // Parametrize down the length of the brush stroke, copying the stamp onto the image at each location
      for (int j = 0; j < brush_stroke_length; j++) {
        float t = (10.0 * j) / brush_stroke_length;
        timing.setOverrideTime(t);
        int center_x = root_x + (int) effects.tween("wind", effects.RAMPUP);
        int center_y = root_y - (int) effects.tween("stroke", effects.LINEAR);

        for (int x = 0; x < width; x++) {
          for (int y = 0; y < height; y++) {
            if (brush[x][y] == 1) {
              int k = center_x - width / 2 + x;
              int l = center_y - y;
              if (0 <= k && k < old_surface->w && 0 <= l && l < old_surface->h) {

                // Here's the actual pixel change per pixel
                int offset = l * old_surface->w;
                windy_surface_ptrs[i][offset + k] = old_surface_ptr[root_color_address];

                // This is not anti-aliasing. This is just color blending.
                vector<position> positions = {{k - 1, l}, {k + 1, l}, {k, l - 1}};
                for (position p : positions) {
                  if (0 <= p.x && p.x < old_surface->w && 0 <= p.y && p.y < old_surface->h) {
                    int blend_address = p.y * old_surface->w + p.x;
                    Uint8 a,r,g,b;

                    SDL_GetRGBA(windy_surface_ptrs[i][blend_address], windy_surfaces[i]->format, &r, &g, &b, &a);
                    Uint8 new_a, new_r, new_g, new_b;
                    new_r = blend_factor * root_r + (1 - blend_factor) * r;
                    new_g = blend_factor * root_g + (1 - blend_factor) * g;
                    new_b = blend_factor * root_b + (1 - blend_factor) * b;
                    new_a = blend_factor * root_a + (1 - blend_factor) * a;
                    windy_surface_ptrs[i][blend_address] = SDL_MapRGBA(windy_surfaces[i]->format, new_r, new_g, new_b, new_a);
                    
                    // if (math_utils.randomInt(0,500) == 88 && abs(g - root_g) > 30) {
                    //   printf("Old %d, %d, %d, %d\n", r, g, b, a);
                    //   printf("Root %d, %d, %d, %d\n", root_r, root_g, root_b, root_a);
                    //   printf("New %d, %d, %d, %d\n", new_r, new_g, new_b, new_a);
                    // }
                    
                    // Uint8 new_a = blend_factor * root_a > a ? blend_factor * root_a : a;
                    // windy_surface_ptrs[i][blend_address] = SDL_MapRGBA(windy_surfaces[i]->format, root_r, root_g, root_b, new_a);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  for (int i = 0; i < wind_frames.size(); i++) {
    SDL_UnlockSurface(windy_surfaces[i]);
    graphics.addImageFromSurface("windy_surface_" + to_string(i), windy_surfaces[i]);
  }

  timing.removeOverrideTime();
}

void logic() {
  if (mode == "displayMode") {
    displayModeLogic();
  } else if (mode == "selectGrass") {
    selectGrassModeLogic();
  }
}

void displayModeLogic() {
  input.processInput();

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

  // Control the frame yourself
  // if (input.keyPressed("z") > 0) {
  //   frame_count -= 1;
  //   if (frame_count < 0) frame_count = 0;
  // }
  // if (input.keyPressed("x") > 0) {
  //   frame_count += 1;
  //   if (frame_count > wind_frames.size() - 1) frame_count = wind_frames.size() - 1;
  // }

  // Random walk with a decreasing probability of going to the right as you go to the right
  // int wind_direction = hot_config.getInt("grass", "wind_strength") > 0 ? 1 : -1;
  // if (timing.since("animation_delay") > hot_config.getFloat("animation", "animation_delay")) {
  //   int r = math_utils.randomInt(0,100);
  //   if (r < 30 + 5 * frame_count) {
  //     frame_count -= wind_direction;
  //   } else {
  //     frame_count += wind_direction;
  //   }
  //   if (frame_count < 0) frame_count = 0;
  //   if (frame_count > wind_frames.size() - 1) frame_count = wind_frames.size() - 1;
  //   timing.mark("animation_delay");
  // }

  // Random walk where the overrunning the ends causes a bounce back to the middle
  int wind_direction = hot_config.getInt("grass", "wind_strength") > 0 ? 1 : -1;
  if (timing.since("animation_delay") > hot_config.getFloat("animation", "animation_delay")) {
    int r = math_utils.randomInt(0,100);
    if (r < 35) {
      frame_count -= wind_direction;
    } else {
      frame_count += wind_direction;
    }
    if (frame_count < 0) frame_count = 1;
    if (frame_count > wind_frames.size() - 1) frame_count = wind_frames.size() - 2;
    timing.mark("animation_delay");
  }

  if (input.keyPressed("quit") > 0) {
    quit = true;
  }

  if (input.threeQuickKey("escape")) {
    quit = true;
  }
}

void selectGrassModeLogic() {

}

void render() {
  // Clear the screen to dark grey
  graphics.clearScreen("#555555");

  // Switch to 2D drawing mode
  graphics.draw2D();

  //graphics.drawImage(backgrounds[map_number], -camera_x, -camera_y);

  graphics.drawImage("windy_surface_" + to_string(frame_count), -camera_x, -camera_y);

  // Put everything we've drawn on screen
  graphics.updateDisplay();
}

void cleanup() {
}
