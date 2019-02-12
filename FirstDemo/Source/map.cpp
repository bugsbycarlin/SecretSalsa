/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "map.h"

using namespace std;
using namespace Honey;

Map::Map(string map_name) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      extra_path[i][j] = {};
    }
  }

  graphics.addImages("Art/", {
    "rain_streak",
    "rain_splash",
    "white_tile",
    "ice_streak",
    "cloud"
  });

  graphics.addImages("Maps/", {
    map_name,
  });
  this->map_name = map_name;

  map_width = graphics.getWidth(map_name);
  map_height = graphics.getHeight(map_name);

  raining = false;
  ice_shards = false;
  dust_storm = 0;

  initializeRainLayer();

  loadPath();
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

void Map::logic() {
  if (raining) {
    // This makes the rain a lot worse
    // if (timing.since("raining") > 4.0 && rain_angle < 70) {
    //   rain_angle++;
    //   rain_velocity = math_utils.rotateVector(0, 1, -rain_angle);
    // }
    // if (timing.since("raining") > 6.0 && rain_density < 900) {
    //     rain_density++;
    // }

    if (rain_field.size() < rain_density && !ice_shards) {
      int target_x = math_utils.randomInt(0, map_width);
      int target_y = math_utils.randomInt(0, map_height);
      int distance = math_utils.randomInt(800,1000);
      int start_x = target_x + (int) (distance * rain_velocity.x);
      int start_y = target_y - (int) (distance * rain_velocity.y);
      rain_targets.push_back({target_x, target_y});
      rain_field.push_back({start_x, start_y});
    }

    for (int i = 0; i < rain_field.size(); i++) {
      rain_field[i].x -= 15 * rain_velocity.x;
      rain_field[i].y += 15 * rain_velocity.y;

      if (rain_field[i].y > rain_targets[i].y) {
        int target_x = math_utils.randomInt(0, map_width);
        int target_y = math_utils.randomInt(0, map_height);
        int distance = math_utils.randomInt(800,1000);
        if (ice_shards) distance = math_utils.randomInt(400, 500);
        int start_x = target_x + (int) (distance * rain_velocity.x);
        int start_y = target_y - (int) (distance * rain_velocity.y);
        rain_field[i].x = start_x;
        rain_field[i].y = start_y;
        rain_targets[i].x = target_x;
        rain_targets[i].y = target_y;
      }
    }

    if (splash_field.size() < rain_density / 15.0) {
      float target_x = math_utils.randomInt(0, map_width);
      float target_y = math_utils.randomInt(0, map_height);
      splash_field.push_back({target_x, target_y, 0.1});
    }

    for (int i = 0; i < splash_field.size(); i++) {
      splash_field[i].z += 0.05;
      if (splash_field[i].z > 3.0) {
        splash_field[i].x = math_utils.randomInt(0, map_width);
        splash_field[i].y = math_utils.randomInt(0, map_height);
        splash_field[i].z = 0.1;
      }
    }
  }

  if (dust_storm != 0) {
    if (dust_field.size() < rain_density) {
      for (int i = 0; i < 3; i++) {
        int target_x = math_utils.randomInt(0, map_width);
        int target_y = math_utils.randomInt(0, map_height);
        int distance = math_utils.randomInt(600,800);
        int start_x = target_x + (int) (distance * dust_storm);
        int start_y = target_y - math_utils.randomInt(0, 100);
        dust_targets.push_back({target_x, target_y});
        dust_field.push_back({start_x, start_y});
        dust_orig_x.push_back(start_x);
      }
    } else if (!dust_storm) {
      dust_field = {};
      dust_targets = {};
      dust_orig_x = {};
    }

    for (int i = 0; i < dust_field.size(); i++) {
      dust_field[i].x += dust_storm * hot_config.getFloat("game", "dust_storm_speed");
      if (dust_field[i].y > dust_targets[i].y) dust_field[i].y -= 4;
      // if (dust_field[i].y < dust_targets[i].y) dust_field[i].y += 4;
    }
  }
}

void Map::draw(int x, int y) {
  graphics.drawImage(map_name, x, y);

  if (raining) {
    if (timing.since("raining") > 4.0) {
      graphics.drawImage("rain_layer", 0, 0);
    } else if (timing.since("raining") >= 2.0) {
      graphics.setColor("#FFFFFF", (timing.since("raining") / 2.0) - 1.0);
      graphics.drawImage("rain_layer", 0, 0);
      graphics.setColor("#FFFFFF", 1.0);
    }

    for (point p : splash_field) {
      graphics.setColor("#FFFFFF", 1.0 - p.z / 3.0);
      graphics.drawImage("rain_splash", p.x + x, p.y + y, true, 0, rain_scale * p.z);
      graphics.setColor("#FFFFFF", 1.0);
    }
  }
}

void Map::overlayer(int x, int y) {
  graphics.setColor("#FFFFFF", 1.0);
  if (raining) {
    if (ice_shards) {
      for (position p : rain_field) {
        graphics.drawImage("ice_streak", p.x + x, p.y + y, true, rain_angle, rain_scale);
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