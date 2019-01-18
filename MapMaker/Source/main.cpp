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
void updateOverlay();
void updateText();
void load();
void save();
  
int mode;
bool quit;

int map_number;
int map_width;
int map_height;

std::vector<std::vector<int>> map_pixel_status;
std::vector<std::vector<int>> save_pixels;

int camera_x;
int camera_y;
float camera_vx;
float camera_vy;
const float camera_velocity_decay = 0.95;

const int max_stamp_radius = 100;
const int default_stamp_radius = 20;
int stamp_radius;

bool eraser;

int cursor_x;
int cursor_y;

vector<string> backgrounds = {};

SDL_Surface* overlay_surface;

const int num_info_boxes = 8;
Textbox* info_box[num_info_boxes];

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

  for (int i = 0; i < num_info_boxes; i++) {
    info_box[i] = new Textbox(
      "Fonts/avenir_black.ttf",
      20,
      "X",
      (position) {hot_config.getInt("layout", "font_x"), hot_config.getInt("layout", "font_y") + 30 * i},
      "#000000"
    );
  }

  for (directory_iterator itr("Maps/"); itr != directory_iterator{}; ++itr) {
    if (is_regular_file(itr->path())) {
      backgrounds.push_back(itr->path().stem().c_str());
      printf("%s\n", itr->path().filename().c_str());
    }
  }

  graphics.addImages("Maps/", backgrounds);

  graphics.addImage("cursor", "Art/cursor.png");
  graphics.addImage("eraser", "Art/eraser.png");

  map_number = 0;

  camera_x = 0;
  camera_y = 0;

  camera_vx = 0;
  camera_vy = 0;

  cursor_x = -500;
  cursor_y = -500;

  stamp_radius = default_stamp_radius;

  eraser = false;

  updateText();

  reset();
}

void updateText() {
  info_box[0]->setText("(Up) (Down) (Left) (Right) Move Around");
  info_box[1]->setText((eraser) ? "(E) Use Stamp" : "(E) Use Eraser");
  info_box[2]->setText("(-) Smaller Stamp");
  info_box[3]->setText("(+) Bigger Stamp");
  info_box[4]->setText("(<) Previous Map");
  info_box[5]->setText("(>) Next Map");
  info_box[6]->setText("(S) Save");
  info_box[7]->setText(" ");
  if (timing.locked("saved")) {
    info_box[7]->setText("Saved Outputs/" + backgrounds[map_number] + "_path.txt");
  }
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

  for (int k = 0; k < map_width; k++) {
    for (int l = 0; l < map_height; l++) {
      map_pixel_status[k][l] = 0;
    }
  }

  load();

  if (overlay_surface != NULL) {
    SDL_FreeSurface(overlay_surface);
  }

  Uint32 rmask, gmask, bmask, amask;
  rmask = 0x00ff0000;
  gmask = 0x0000ff00;
  bmask = 0x000000ff;
  amask = 0xff000000;

  overlay_surface = SDL_CreateRGBSurface(0, map_width, map_height, 32, rmask, gmask, bmask, amask);

  updateOverlay();
}

void load() {
  std::ifstream input_file("Outputs/" + backgrounds[map_number] + "_path.txt");

  string line;
  int count = 0;
  while (getline(input_file, line)) {
    if (count > 0) {
      std::vector<std::string> words;
      boost::split(words, line, boost::is_any_of(","), boost::token_compress_on);
      int x = stoi(words[0]);
      int y = stoi(words[1]);
      int r = stoi(words[2]);
      for (int i = x - r; i < x + r; i++) {
        for (int j = y - r; j < y + r; j++) {
          if (i >= 0 && i < map_width && j >= 0 && j < map_width
            && math_utils.distance(i,j,x,y) < r) {
            map_pixel_status[i][j] = 1;
          }
        }
      }
    }
    count++;
  }
}

void save() {
  std::ofstream output_file;
  output_file.open("Outputs/" + backgrounds[map_number] + "_path.txt");

  output_file << "page,1,0,0\n";

  int total_pixels = 0;
  int total_shapes = 0;
  int remaining_pixels = 0;

  int min_map_x = 20000000;
  int max_map_x = 0;
  int min_map_y = 20000000;
  int max_map_y = 0;

  save_pixels.resize(map_width);
  for (int k = 0; k < map_width; k++) {
    save_pixels[k].resize(map_height);
  }

  for (int k = 0; k < map_width; k++) {
    for (int l = 0; l < map_height; l++) {

      save_pixels[k][l] = 0;

      if(map_pixel_status[k][l] == 1) {
        total_pixels += 1;
        remaining_pixels += 1;

        if (k < min_map_x) min_map_x = k;
        if (k > max_map_x) max_map_x = k;
        if (l < min_map_y) min_map_y = l;
        if (l > max_map_y) max_map_y = l;
      }
    }
  }

  printf("Remaining pixels: %d\n", remaining_pixels);

  for (int radius = max_stamp_radius; radius >= 0; radius -= 5) {
    int counter = 0;
    int samples = 100000;
    printf("Testing for radius %d\n", radius);
    if (radius <= 50) samples = 250000;
    if (radius <= 30) samples = 1000000;
    if (radius == 0) radius = 3;
    while (counter < samples) {
      counter += 1;
      int k = math_utils.randomInt(min_map_x, max_map_x);
      int l = math_utils.randomInt(min_map_y, max_map_y);
      if (counter % 25000 == 0) printf("Counter %d\n", counter);
      if (map_pixel_status[k][l] == 1 && save_pixels[k][l] != 1) {
        bool inside = true;
        for(int i = k - radius; i < k + radius; i++) {
          for(int j = l - radius; j < l + radius; j++) {
            if (i < min_map_x
              || i > max_map_x
              || j < min_map_y
              || j > max_map_y
              || (map_pixel_status[i][j] == 0 && math_utils.distance(k, l, i, j) <= radius)) {
              inside = false;
              goto after_loop;
            }
          }
        }
        after_loop: if (inside) {
          total_shapes += 1;
          string output = to_string(k) + "," + to_string(l) + "," + to_string(radius) + "\n";
          printf("%s", output.c_str());
          output_file << output.c_str();
          for(int i = k - radius; i < k + radius; i++) {
            for(int j = l - radius; j < l + radius; j++) {
              if (math_utils.distance(k, l, i, j) <= radius && save_pixels[i][j] != 1) {
                save_pixels[i][j] = 1;
                remaining_pixels -= 1;
              }
            }
          }
        }
      }
    }
    printf("Remaining pixels: %d\n", remaining_pixels);
    printf("Total shapes: %d\n", total_shapes);
  }

  for (int k = min_map_x; k <= max_map_x; k++) {
    for (int l = min_map_y; l <= max_map_y; l++) {
      if (map_pixel_status[k][l] == 1 && save_pixels[k][l] != 1) {
        map_pixel_status[k][l] = 2;
      }
    }
  }

  printf("Total pixels: %d\n", total_pixels);

  output_file.close();

  printf("Saved Outputs/%s_path.txt\n", backgrounds[map_number].c_str());
}

void updateOverlay() {
  SDL_LockSurface(overlay_surface);
  unsigned int *overlay_ptr = (unsigned int*)overlay_surface->pixels;

  for (int k = 0; k < map_width; k++) {
    for (int l = 0; l < map_height; l++) {
      int overlay_offset = l * overlay_surface->w;
      Uint32 mask = 0x77111111;
      if (map_pixel_status[k][l] == 1) {
        mask = 0x00111111;
      } else if (map_pixel_status[k][l] == 2) {
        mask = 0x44111111;
      }

      overlay_ptr[overlay_offset + k] = mask;
    }
  }

  SDL_UnlockSurface(overlay_surface);

  graphics.removeImage("overlay");
  graphics.addImageFromSurface("overlay", overlay_surface);
}

void logic() {
  input.processInput();

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
              map_pixel_status[k][l] = 1;
              if (eraser) map_pixel_status[k][l] = 0;
            }
          }
        }
      }
      updateOverlay();
    }
  }

  if (timing.since("saved") < 3.2) {
    updateText();
  }

  if (!timing.locked("saved") && input.keyPressed("s") > 0) {
    save();
    timing.lock("saved", 3);
    updateOverlay();
  }

  if (!timing.locked("saved") && input.keyPressed(",") > 0) {
    map_number -= 1;
    if (map_number < 0) {
      map_number = backgrounds.size() - 1;
    }
    reset();
  }

  if (!timing.locked("saved") && input.keyPressed(".") > 0) {
    map_number += 1;
    if (map_number >= backgrounds.size()) {
      map_number = 0;
    }
    reset();
  }

  if (input.keyPressed("=") > 0) {
    if (stamp_radius <= max_stamp_radius) stamp_radius += 5;
  }

  if (input.keyPressed("-") > 0) {
    if (stamp_radius > 5) stamp_radius -= 5;
  }

  if (input.keyPressed("e") > 0) {
    if (eraser) {
      eraser = false;
    } else {
      eraser = true;
    }

    updateText();
  }

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

  graphics.drawImage((eraser) ? "eraser" : "cursor", cursor_x, cursor_y, true, 0, stamp_radius / 50.0);

  for (int i = 0; i < num_info_boxes; i++) {
    info_box[i]->draw();
  }

  // Put everything we've drawn on screen
  graphics.updateDisplay();
}

void cleanup() {
  for (int i = 0; i < num_info_boxes; i++) {
    delete info_box[i];
  }
  delete overlay_surface;
}
