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

  graphics.addImages("Maps/", {
    map_name,
  });
  this->map_name = map_name;

  map_width = graphics.getWidth(map_name);
  map_height = graphics.getHeight(map_name);

  loadPath();
}

void Map::loadPath() {
  std::ifstream input_file("Scripts/" + map_name + "_path.txt");

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

void Map::draw(int x, int y) {
  graphics.drawImage(map_name, x, y);
}

Map::~Map() {
}