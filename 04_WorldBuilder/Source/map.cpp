/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "map.h"

using namespace std;
using namespace tinyxml2;
using namespace Honey;

Map::Map(string map_name) {
  this->map_name = map_name;
  this->map_path = hot_config.getString("system", "map_root_folder") + "/" + map_name + "/";

  scale = 1;

  loadConfig();

  loadSheets();
}

void Map::loadConfig() {
  string config_file = map_path + map_name + ".txt";

  XMLDocument doc;
  XMLError result = doc.LoadFile(config_file.c_str());

  if (result != XML_SUCCESS) {
    printf("Map config load failed entirely! Failed to load and parse file at path %s\n", config_file.c_str());
    exit(1);
  }

  XMLElement* element = doc.FirstChildElement();
  string node_value;
  while (element != nullptr) {
    node_value = element->Value();
    string node_text(element->GetText());
    boost::algorithm::trim(node_text);

    std::vector<std::string> things;
    boost::split(things, node_text, boost::is_any_of(","), boost::token_compress_on);

    printf("There are %lu %s. The first one is %s\n", things.size(), node_value.c_str(), things[0].c_str());
    if (node_value == "sheets") {
      sheet_names = things;
    } else if (node_value == "layers") {
      layer_names = things;
    } else if (node_value == "doodads") {
      doodad_names = things;
    } else if (node_value == "scale") {
      int result = element->QueryIntText(&scale);
      if (result != XML_SUCCESS) {
        printf("Map config failed to parse int value for scale tag.\n");
        exit(1);
      }
      printf("The scale is %d\n", scale);
    }

    element = element->NextSiblingElement();
  }

  if (sheet_names.size() == 0) {
    printf("Map config was missing sheets! Quitting.\n");
    exit(1);
  }

  if (layer_names.size() == 0) {
    printf("Map config was missing layers! Quitting.\n");
    exit(1);
  }

  layer_names.push_back("selection");
}

void Map::loadSheets() {
  graphics.addImages(map_path, doodad_names);
  map_width = 0;
  map_height = 0;
  timing.mark("long_calculation");
  for (string sheet : sheet_names) {
    for (string layer : layer_names) {
      string path = map_path + sheet + "_" + layer + ".png";
      printf("Path is %s\n", path.c_str());
      if (!boost::filesystem::exists(path)) {
        if (map_width == 0 || map_height == 0) {
          printf("Error for %s. The first layer in the list must have a width and a height. Quitting.\n", path.c_str());
          exit(1);
        }
        SDL_Surface* sheet_surface = SDL_CreateRGBSurface(0, map_width, map_height, 32, rmask, gmask, bmask, amask);
        sheets[sheet + "_" + layer] = sheet_surface;
        graphics.addImageFromSurface(sheet + "_" + layer, sheet_surface);
      } else {
        SDL_Surface* source_surface = IMG_Load(path.c_str());
        if (map_width != 0 && map_width != source_surface->w * scale) {
          printf("Error for %s. Layers do not have matching width! Quitting.\n", path.c_str());
          exit(1);
        }
        if (map_height != 0 && map_height != source_surface->h * scale) {
          printf("Error for %s. Layers do not have matching height! Quitting.\n", path.c_str());
          exit(1);
        }
        map_width = source_surface->w * scale;
        map_height = source_surface->h * scale;

        SDL_Surface* sheet_surface = SDL_CreateRGBSurface(0, map_width, map_height, 32, rmask, gmask, bmask, amask);

        SDL_LockSurface(sheet_surface);
        unsigned int *source_surface_ptr = (unsigned int*)source_surface->pixels;
        unsigned int *sheet_surface_ptr = (unsigned int*)sheet_surface->pixels;

        // Copy the old surface to the new at scale
        for (int k = 0; k < source_surface->w; k++) {
          for (int l = 0; l < source_surface->h; l++) {
            int source_address = l * source_surface->w + k;

            for (int i = 0; i < scale; i++) {
              for (int j = 0; j < scale; j++) {
                int sheet_address = (scale * l + j) * sheet_surface->w + (scale * k + i);
                sheet_surface_ptr[sheet_address] = source_surface_ptr[source_address];
              }
            }
          }

          if (timing.since("long_calculation") > 0.1) {
            timing.mark("long_calculation");
            drawWaiting();
          }
        }

        SDL_UnlockSurface(sheet_surface);
        SDL_FreeSurface(source_surface);

        sheets[sheet + "_" + layer] = sheet_surface;
        graphics.addImageFromSurface(sheet + "_" + layer, sheet_surface);
        printf("Added one. Size is %d, %d\n", sheet_surface->w, sheet_surface->h);
      }
    }
  }
}

void Map::logic() {

}

void Map::draw(int x, int y) {
  // This is wrong, and will draw all the sheets and layers right on top of each other
  for (string sheet : sheet_names) {
    for (string layer : layer_names) {
      graphics.drawImage(sheet + "_" + layer, x, y);
    }
  }
}

void Map::drawWaiting() {
  wait_step += 1;
  wait_step = wait_step % 2;
  graphics.setColor("#FFFFFF", 1.0);
  graphics.drawImage("waiting_wheel_" + to_string(wait_step), 1280 - 100, 60);
  graphics.updateDisplay();
}











void Map::loadPath() {
  std::ifstream input_file("Maps/" + map_name + "_path.txt");

  string line;
  int count = 0;
  while (getline(input_file, line)) {
    if (count > 0) {
      std::vector<std::string> words;
      boost::split(words, line, boost::is_any_of(","), boost::token_compress_on);
      int x = stoi(words[0]);
      int y = stoi(words[1]);
      int r = stoi(words[2]);
      if (r != 0) {
        path.push_back({x,y,r});
      } else {
        int zone_x = x / (map_width / 8.0);
        int zone_y = y / (map_height / 8.0);
        extra_path[zone_x][zone_y].push_back({x,y,r});
      }
    }
    count++;
  }
}

void Map::initializeRainLayer() {
  Uint32 rmask, gmask, bmask, amask;
  rmask = 0x00ff0000;
  gmask = 0x0000ff00;
  bmask = 0x000000ff;
  amask = 0xff000000;

  int width = hot_config.getInt("layout", "screen_width");
  int height = hot_config.getInt("layout", "screen_height");
  SDL_Surface* rain_surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);

  SDL_LockSurface(rain_surface);
  unsigned int *rain_surface_ptr = (unsigned int*)rain_surface->pixels;

  for (int k = 0; k < width; k++) {
    for (int l = 0; l < height; l++) {
      rain_surface_ptr[l * rain_surface->w + k] = 0x44444455;
    }
  }

  SDL_UnlockSurface(rain_surface);
  graphics.addImageFromSurface("rain_layer", rain_surface);
}

void Map::startRain() {
  raining = true;
  rain_field = {};
  rain_targets = {};
  splash_field = {};
  rain_velocity = math_utils.rotateVector(0, 1, -rain_angle);
  timing.mark("raining");
}

void Map::stopRain() {
  raining = false;
  rain_field = {};
  rain_targets = {};
  splash_field = {};
  timing.remove("raining");
}

bool Map::checkPath(int x, int y) {
  for (map_circle circle : path) {
    if (math_utils.distance(x, y, circle.x, circle.y) <= circle.r) {
      return true;
    }
  }
  int zone_x = x / (map_width / 8.0);
  int zone_y = y / (map_height / 8.0);
  for (map_circle circle : extra_path[zone_x][zone_y]) {
    if (x == circle.x && y == circle.y) {
      return true;
    }
  }
  return false;
}

// void Map::logic() {
//   if (raining) {
//     // This makes the rain a lot worse
//     // if (timing.since("raining") > 4.0 && rain_angle < 70) {
//     //   rain_angle++;
//     //   rain_velocity = math_utils.rotateVector(0, 1, -rain_angle);
//     // }
//     // if (timing.since("raining") > 6.0 && rain_density < 900) {
//     //     rain_density++;
//     // }

//     if (rain_field.size() < rain_density && !ice_shards) {
//       int target_x = math_utils.randomInt(0, map_width);
//       int target_y = math_utils.randomInt(0, map_height);
//       int distance = math_utils.randomInt(800,1000);
//       int start_x = target_x + (int) (distance * rain_velocity.x);
//       int start_y = target_y - (int) (distance * rain_velocity.y);
//       rain_targets.push_back({target_x, target_y});
//       rain_field.push_back({start_x, start_y});
//     }

//     for (int i = 0; i < rain_field.size(); i++) {
//       rain_field[i].x -= 15 * rain_velocity.x;
//       rain_field[i].y += 15 * rain_velocity.y;

//       if (rain_field[i].y > rain_targets[i].y) {
//         int target_x = math_utils.randomInt(0, map_width);
//         int target_y = math_utils.randomInt(0, map_height);
//         int distance = math_utils.randomInt(800,1000);
//         if (ice_shards) distance = math_utils.randomInt(400, 500);
//         int start_x = target_x + (int) (distance * rain_velocity.x);
//         int start_y = target_y - (int) (distance * rain_velocity.y);
//         rain_field[i].x = start_x;
//         rain_field[i].y = start_y;
//         rain_targets[i].x = target_x;
//         rain_targets[i].y = target_y;
//       }
//     }

//     if (splash_field.size() < rain_density / 15.0) {
//       float target_x = math_utils.randomInt(0, map_width);
//       float target_y = math_utils.randomInt(0, map_height);
//       splash_field.push_back({target_x, target_y, 0.1});
//     }

//     for (int i = 0; i < splash_field.size(); i++) {
//       splash_field[i].z += 0.05;
//       if (splash_field[i].z > 3.0) {
//         splash_field[i].x = math_utils.randomInt(0, map_width);
//         splash_field[i].y = math_utils.randomInt(0, map_height);
//         splash_field[i].z = 0.1;
//       }
//     }
//   }

//   if (dust_storm != 0) {
//     if (dust_field.size() < rain_density) {
//       for (int i = 0; i < 3; i++) {
//         int target_x = math_utils.randomInt(0, map_width);
//         int target_y = math_utils.randomInt(0, map_height);
//         int distance = math_utils.randomInt(600,800);
//         int start_x = target_x + (int) (distance * dust_storm);
//         int start_y = target_y - math_utils.randomInt(0, 100);
//         dust_targets.push_back({target_x, target_y});
//         dust_field.push_back({start_x, start_y});
//         dust_orig_x.push_back(start_x);
//       }
//     }

//     for (int i = 0; i < dust_field.size(); i++) {
//       dust_field[i].x += dust_storm * hot_config.getFloat("game", "dust_storm_speed");
//       if (dust_field[i].y > dust_targets[i].y) dust_field[i].y -= 4;
//       // if (dust_field[i].y < dust_targets[i].y) dust_field[i].y += 4;
//     }
//   } else {
//     dust_field = {};
//     dust_targets = {};
//     dust_orig_x = {};
//   }
// }

// void Map::draw(int x, int y) {
//   graphics.drawImage(map_name, x, y);

//   if (raining) {
//     if (timing.since("raining") > 4.0) {
//       graphics.drawImage("rain_layer", 0, 0);
//     } else if (timing.since("raining") >= 2.0) {
//       graphics.setColor("#FFFFFF", (timing.since("raining") / 2.0) - 1.0);
//       graphics.drawImage("rain_layer", 0, 0);
//       graphics.setColor("#FFFFFF", 1.0);
//     }

//     for (point p : splash_field) {
//       graphics.setColor("#FFFFFF", 1.0 - p.z / 3.0);
//       graphics.drawImage("rain_splash", p.x + x, p.y + y, true, 0, rain_scale * p.z);
//       graphics.setColor("#FFFFFF", 1.0);
//     }
//   }
// }

void Map::overlayer(int x, int y) {
  graphics.setColor("#FFFFFF", 1.0);

  graphics.drawImage(map_name + "_overlay", x, y);

  if (raining) {
    if (ice_shards) {
      for (position p : rain_field) {
        graphics.drawImage("ice_streak", p.x + x, p.y + y, true, rain_angle, 3 * rain_scale);
      }
    } else {
      for (position p : rain_field) {
        graphics.drawImage("rain_streak", p.x + x, p.y + y, true, rain_angle, rain_scale);
      }
    }

    graphics.setColor("#FFFFFF", 1.0 / (4.0 * timing.since("raining")));
    graphics.drawImage("white_tile", map_width / 2.0, map_height / 2.0, true, 0, map_width / 64.0);
    graphics.setColor("#FFFFFF", 1.0);
  }


  if (dust_storm != 0) {
    for (int i = 0; i < dust_field.size(); i++) {
      position p = dust_field[i];
      int orig_x = dust_orig_x[i];
      float scale = std::min(abs(p.x - orig_x) / 600.0, 0.5);
      graphics.drawImage("cloud", p.x + x, p.y + y, true, 8 * (p.x - orig_x), scale);
    }
  }
}

Map::~Map() {
}