/*
  Secret Salsa
  Matthew Carlin
  Copyright 2019
*/

#include "grass_maker.h"

using namespace std;
using namespace Honey;

void debug(int x) {
  printf("Here %d\n", x);
}

int GrassMaker::unique_count = 0;

GrassMaker::GrassMaker(std::vector<std::vector<int>> selection_pixels, function<void(int)> progress_action) {
  unique_count++;

  wind_frames = {-0.5, -0.25, 0, 0.25, 0.5, 0.625, 0.75, 0.875, 1.0};

  int map_width = hot_config.getInt("layout", "selection_width");
  int map_height = hot_config.getInt("layout", "selection_height");

  int num_calculations = 0;

  for (int i = 0; i < wind_frames.size(); i++) {
    SDL_Surface* x = SDL_CreateRGBSurface(0, map_width, map_height, 32, rmask, gmask, bmask, amask);
    windy_surfaces.push_back(x);
  }

  vector<unsigned int *>windy_surface_ptrs;
  for (int i = 0; i < wind_frames.size(); i++) {
    SDL_LockSurface(windy_surfaces[i]);
    windy_surface_ptrs.push_back((unsigned int *)windy_surfaces[i]->pixels);
  }

  // Count pixels needed to try
  int num_pixels_in_selection = 0;
  for (int k = 0; k < map_width; k++) {
    for (int l = 0; l < map_height; l++) {
      if (selection_pixels[k][l] == 1) {
        num_pixels_in_selection += 1;
      }
    }
  }
  printf("Number of pixels in selection is %d\n", num_pixels_in_selection);

  timing.setOverrideTime(0.0);

  // Set up the parameters
  int wind_strength_standard = hot_config.getInt("grass", "wind_strength");
  int brush_stroke_length_standard = hot_config.getInt("grass", "brush_stroke_length");
  int width = hot_config.getInt("grass", "brush_width");
  int height = hot_config.getInt("grass", "brush_height");
  int density = hot_config.getInt("grass", "brush_density");
  float opacity = hot_config.getFloat("grass", "brush_opacity");
  float blend_factor = hot_config.getFloat("grass", "brush_blend_factor");

  vector<string> grass_colors;
  string grass_color_string = hot_config.getString("grass", "grass_colors");
  boost::split(grass_colors, grass_color_string, boost::is_any_of(","), boost::token_compress_on);

  // Number of strokes is based on density now
  int brush_stroke_count = (int) (num_pixels_in_selection * hot_config.getFloat("grass", "brush_stroke_density"));
  printf("Brush stroke count is %d\n", brush_stroke_count);

  // Grab a series of points. We're going to sort these in Y order so they draw correctly.
  vector<position> root_positions;
  int sample_brush_strokes = brush_stroke_count;
  while(sample_brush_strokes > 0) {
    int x = math_utils.randomInt(0, map_width);
    int y = math_utils.randomInt(0, map_height);
    if (selection_pixels[x][y] == 1) {
      sample_brush_strokes -= 1;
      root_positions.push_back({x, y});
    }

    num_calculations += 1;
    if (num_calculations % 50000 == 0) {
      progress_action(num_calculations);
    }
    debug(1);
  }

  sort(root_positions.begin(), root_positions.end());
  debug(2);

  // Do a series of brush strokes
  for (int c = 0; c < brush_stroke_count; c++) {
    debug(3);
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
    
    intColor root_color = graphics.parseIntColor(grass_colors[math_utils.randomInt(0, grass_colors.size())]);
    printf("Color is %d,%d,%d\n", root_color.r, root_color.g, root_color.b);

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
              if (0 <= k && k < map_width && 0 <= l && l < map_height) {

                // Here's the actual pixel change per pixel
                int address = l * map_width + k;
                windy_surface_ptrs[i][address] = SDL_MapRGBA(windy_surfaces[i]->format,
                  root_color.r, root_color.g, root_color.b, 255);

                num_calculations += 1;
                if (num_calculations % 200000 == 0) {
                  progress_action(num_calculations);
                }

                // This is not anti-aliasing. This is just color blending.
                // vector<position> positions = {{k - 1, l}, {k + 1, l}, {k, l - 1}};
                // for (position p : positions) {
                //   if (0 <= p.x && p.x < old_surface->w && 0 <= p.y && p.y < old_surface->h) {
                //     int blend_address = p.y * old_surface->w + p.x;
                //     Uint8 a,r,g,b;

                //     SDL_GetRGBA(windy_surface_ptrs[i][blend_address], windy_surfaces[i]->format, &r, &g, &b, &a);
                //     Uint8 new_a, new_r, new_g, new_b;
                //     new_r = blend_factor * root_r + (1 - blend_factor) * r;
                //     new_g = blend_factor * root_g + (1 - blend_factor) * g;
                //     new_b = blend_factor * root_b + (1 - blend_factor) * b;
                //     new_a = blend_factor * root_a + (1 - blend_factor) * a;
                //     windy_surface_ptrs[i][blend_address] = SDL_MapRGBA(windy_surfaces[i]->format, new_r, new_g, new_b, new_a);

                //     // if (math_utils.randomInt(0,500) == 88 && abs(g - root_g) > 30) {
                //     //   printf("Old %d, %d, %d, %d\n", r, g, b, a);
                //     //   printf("Root %d, %d, %d, %d\n", root_r, root_g, root_b, root_a);
                //     //   printf("New %d, %d, %d, %d\n", new_r, new_g, new_b, new_a);
                //     // }

                //     // Uint8 new_a = blend_factor * root_a > a ? blend_factor * root_a : a;
                //     // windy_surface_ptrs[i][blend_address] = SDL_MapRGBA(windy_surfaces[i]->format, root_r, root_g, root_b, new_a);
                //   }
                // }
              }
            }
          }
        }
      }
    }
  }

  debug(8);

  vector<string> frames = {};
  for (int i = 0; i < wind_frames.size(); i++) {
    SDL_UnlockSurface(windy_surfaces[i]);
    string frame_name = "windy_surface_" + to_string(unique_count) + "_" + to_string(i);
    graphics.addImageFromSurface(frame_name, windy_surfaces[i]);
    frames.push_back(frame_name);
    printf("%s\n", frames[frames.size() - 1].c_str());
  }

  debug(9);

  result = new Drawable("", map_width / 2, map_height / 2);
  result->addAnimation("default", frames);

  timing.removeOverrideTime();
}

GrassMaker::~GrassMaker() {
}