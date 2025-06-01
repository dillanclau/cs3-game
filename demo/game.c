#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "forces.h"
#include "sdl_wrapper.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {750, 500};
const vector_t CENTER = {375, 250};

const vector_t START_POS = {500, 30};
const vector_t RESET_POS = {500, 45};
const vector_t BASE_OBJ_VEL = {30, 0};
const double EXTRA_VEL_MULT = 10;
const double VEL_MULT_PROB = 0.2;

const double OUTER_RADIUS = 15;
const double INNER_RADIUS = 15;
const size_t OBSTACLE_HEIGHT = 30;
const vector_t OBS_WIDTHS = {30, 70};
const vector_t OBS_SPACING = {120, 350};

const size_t SPIRIT_NUM_POINTS = 20;

const color_t OBS_COLOR = (color_t){0.2, 0.2, 0.3};
const color_t SPIRIT_COLOR = (color_t){0.1, 0.9, 0.2};
const color_t TEXT_COLOR = (color_t){1, 0, 0};

// constants to create platforms
const size_t NUM_MAP = 3;
const size_t BRICK_WIDTH = 20;
const size_t BRICK_NUM[NUM_MAP] = {10, 10, 10};
size_t BRICKS1[10][4] = {{160, 425, 320, BRICK_WIDTH},
                         {560, 425, 150, BRICK_WIDTH},
                         {425, 300, 650, BRICK_WIDTH},
                         {325, 200, 650, BRICK_WIDTH},
                         {180, 75, 175, BRICK_WIDTH},
                         {500, 75, 175, BRICK_WIDTH},
                         {730, 330, 40, 60},
                         {30, 235, 60, 70},
                         {730, 90, 40, 60},
                         {715, 35, 70, 70}};
// size_t BRICKS2[][]
// size_t BRICKS3[][]

const size_t LAVA_WIDTH = 7;
const size_t LAVA_NUM[NUM_MAP] = {4, 0, 0};
size_t LAVA1[4][4] = {{180, 10, 165, LAVA_WIDTH},
                      {500, 90, 165, LAVA_WIDTH},
                      {500, 310, 100, LAVA_WIDTH},
                      {250, 310, 175, LAVA_WIDTH}};

const int16_t H_STEP = 50;
const int16_t V_STEP = 30;
const size_t ROWS = 50;

const size_t BODY_ASSETS = 2;

const char *SPIRIT_FRONT_PATH = "assets/waterspiritfront.png";
const char *BACKGROUND_PATH = "assets/dungeonbackground.png";
const char *BRICK_PATH = "assets/bricks.png";
const char *BRICK_PATH1 = "assets/log.png";
const char *PAUSE_FILEPATH = "assets/pause.png";
const char *FONT_FILEPATH = "assets/Cascadia.ttf";

typedef enum {
  LEVEL1 = 1,
  LEVEL2 = 2,
  LEVEL3 = 3,
  HOMEPAGE = 4,
} screen_t;
const char *BRICK_PATH = "assets/brick_texture.png";
const char *LAVA_PATH = "assets/lava.png";

struct state {
  body_t *spirit;
  scene_t *scene;
  int16_t points;
  screen_t current_screen;
  bool pause;
  body_t *pause_body;
};

body_t *make_obstacle(size_t w, size_t h, vector_t center, char *info) {
  list_t *c = list_init(4, free);
  vector_t *v1 = malloc(sizeof(vector_t));
  *v1 = (vector_t){0, 0};
  list_add(c, v1);

  vector_t *v2 = malloc(sizeof(vector_t));
  *v2 = (vector_t){w, 0};
  list_add(c, v2);

  vector_t *v3 = malloc(sizeof(vector_t));
  *v3 = (vector_t){w, h};
  list_add(c, v3);

  vector_t *v4 = malloc(sizeof(vector_t));
  *v4 = (vector_t){0, h};
  list_add(c, v4);
  // body_t *obstacle = body_init(c, __DBL_MAX__, OBS_COLOR);
  body_t *obstacle = body_init_with_info(c, __DBL_MAX__, OBS_COLOR, info, NULL);
  body_set_centroid(obstacle, center);
  return obstacle;
}

body_t *make_spirit(double outer_radius, double inner_radius, vector_t center) {
  center.y += inner_radius;
  list_t *c = list_init(SPIRIT_NUM_POINTS, free);
  for (size_t i = 0; i < SPIRIT_NUM_POINTS; i++) {
    double angle = 2 * M_PI * i / SPIRIT_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + inner_radius * cos(angle),
                    center.y + outer_radius * sin(angle)};
    list_add(c, v);
  }
  body_t *spirit = body_init(c, 1, SPIRIT_COLOR);
  return spirit;
}

void wrap_edges(body_t *body) {
  vector_t centroid = body_get_centroid(body);
  if (centroid.x > MAX.x) {
    body_set_centroid(body, (vector_t){MIN.x, centroid.y});
  } else if (centroid.x < MIN.x) {
    body_set_centroid(body, (vector_t){MAX.x, centroid.y});
  } else if (centroid.y > MAX.y) {
    body_set_centroid(body, (vector_t){centroid.x, MIN.y});
  } else if (centroid.y < MIN.y) {
    body_set_centroid(body, (vector_t){centroid.x, MAX.y});
  }
}

void reset_user(body_t *body) { body_set_centroid(body, RESET_POS); }

void reset_user_handler(body_t *body1, body_t *body2, vector_t axis, void *aux,
                        double force_const) {
  reset_user(body1);
}

void player_wrap_edges(state_t *state) {
  body_t *player = scene_get_body(state->scene, 0);
  vector_t centroid = body_get_centroid(player);
  if (centroid.y > MAX.y - V_STEP) {
    state->points += 1;
    reset_user(player);
    fprintf(stdout, "You have %d points!\n", state->points);
  }
}

void go_to_level1(state_t *state) {
  asset_reset_asset_list();
  state->current_screen = LEVEL1;
  return;
}

void go_to_level2(state_t *state) {
  asset_reset_asset_list();
  state->current_screen = LEVEL2;
  return;
}

void go_to_level3(state_t *state) {
  asset_reset_asset_list();
  state->current_screen = LEVEL3;
  return;
}

void go_to_homepage(state_t *state) {
  asset_reset_asset_list();
  state->current_screen = HOMEPAGE;
  SDL_Rect box = (SDL_Rect){.x = MIN.x, .y = MIN.y, .w = MAX.x, .h = MAX.y};
  asset_make_image(BACKGROUND_PATH, box);
  asset_make_text(FONT_FILEPATH,
                  (SDL_Rect){.x = 200, .y = 25, .w = 200, .h = 100}, "HOMEPAGE",
                  TEXT_COLOR);
  asset_make_text(FONT_FILEPATH,
                  (SDL_Rect){.x = 200, .y = 150, .w = 300, .h = 50},
                  "Press 1 to go to Level 1", TEXT_COLOR);
  asset_make_text(FONT_FILEPATH,
                  (SDL_Rect){.x = 200, .y = 250, .w = 300, .h = 50},
                  "Press 2 to go to Level 2", TEXT_COLOR);
  asset_make_text(FONT_FILEPATH,
                  (SDL_Rect){.x = 200, .y = 350, .w = 300, .h = 50},
                  "Press 3 to go to Level 3", TEXT_COLOR);
}

void pause(state_t *state) {
  state->pause = true;
  list_t *body_list = list_init(4, free);
  vector_t *v1 = malloc(sizeof(vector_t));
  *v1 = (vector_t){150, 100};
  list_add(body_list, v1);
  vector_t *v2 = malloc(sizeof(vector_t));
  *v2 = (vector_t){600, 400};
  list_add(body_list, v2);
  vector_t *v3 = malloc(sizeof(vector_t));
  *v3 = (vector_t){150, 400};
  list_add(body_list, v3);
  vector_t *v4 = malloc(sizeof(vector_t));
  *v4 = (vector_t){600, 100};
  list_add(body_list, v4);
  state->pause_body = body_init(body_list, 100, SPIRIT_COLOR);
  body_set_centroid(state->pause_body, CENTER);
  scene_add_body(state->scene, state->pause_body);
  asset_make_image_with_body(PAUSE_FILEPATH, state->pause_body);
}

void unpause(state_t *state) {
  if (state->pause_body) {
    asset_remove_body(state->pause_body);
    body_remove(state->pause_body);
    body_free(state->pause_body);
  }
  state->pause = false;
}

void restart(state_t *state) {
  unpause(state);
  if (state->current_screen == LEVEL1) {
    go_to_level1(state);
  } else if (state->current_screen == LEVEL2) {
    go_to_level2(state);
  } else if (state->current_screen == LEVEL3) {
    go_to_level3(state);
  }
}

void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  body_t *spirit = scene_get_body(state->scene, 0);
  vector_t translation = (vector_t){0, 0};
  if (type == KEY_PRESSED && type != KEY_RELEASED) {
    switch (key) {
    case KEY_1:
      go_to_level1(state);
      break;
    case KEY_2:
      go_to_level2(state);
      break;
    case KEY_3:
      go_to_level3(state);
      break;
    case KEY_H:
      go_to_homepage(state);
      break;
    case KEY_P:
      pause(state);
      break;
    case KEY_R:
      restart(state);
      break;
    case KEY_U:
      unpause(state);
      break;
    case LEFT_ARROW:
      translation.x = -H_STEP;
      break;
    case RIGHT_ARROW:
      translation.x = H_STEP;
      break;
    case UP_ARROW:
      translation.y = V_STEP;
      break;
    case DOWN_ARROW:
      if (body_get_centroid(spirit).y > START_POS.y) {
        translation.y = -V_STEP;
      }
      break;
    }
    vector_t new_centroid = vec_add(body_get_centroid(spirit), translation);
    body_set_centroid(spirit, new_centroid);
  }
}

double rand_double(double low, double high) {
  return (high - low) * rand() / RAND_MAX + low;
}

void make_platforms(state_t *state) {
  // for map 1, will make this more organized later!
  const size_t BRICK_WIDTH = 20;
  const size_t BRICK_NUM = 8;

  // size_t HEIGHTS[ROW_NUM] = {425, 300, 200, 75};
  int BRICKS[BRICK_NUM][3] = {{160, 425, 320}, {560, 425, 150}, {425, 300, 650},
                              {325, 200, 650}, {180, 75, 175},  {500, 75, 175}};
  // {x, y, width}
  for (size_t i = 0; i < BRICK_NUM; i++) {
    vector_t coord = (vector_t){BRICKS[i][0], BRICKS[i][1]};
    body_t *obstacle = make_obstacle(BRICKS[i][2], BRICK_WIDTH, coord);
    void make_platforms(state_t * state, size_t idx) {
      size_t len = BRICK_NUM[0];
      for (size_t i = 0; i < len; i++) {
        vector_t coord = (vector_t){BRICKS1[i][0], BRICKS1[i][1]};
        if (BRICKS1[i][3] == 0) {
          BRICKS1[i][3] = BRICK_WIDTH;
        }
        body_t *obstacle =
            make_obstacle(BRICKS1[i][2], BRICKS1[i][3], coord, "platform");
        scene_add_body(state->scene, obstacle);
        create_collision(state->scene, state->spirit, obstacle,
                         reset_user_handler, NULL, 0, NULL);
        asset_make_image_with_body(BRICK_PATH, obstacle);
      }

      // misc sized blocks
      // {x, y, width, height}
      const size_t NUM_OBST = 4;
      int OBST[NUM_OBST][4] = {{730, 330, 40, 60},
                               {30, 235, 60, 70},
                               {730, 90, 40, 60},
                               {715, 35, 70, 70}};

      for (size_t i = 0; i < NUM_OBST; i++) {
        vector_t coord = (vector_t){OBST[i][0], OBST[i][1]};
        body_t *obstacle = make_obstacle(OBST[i][2], OBST[i][3], coord);
        scene_add_body(state->scene, obstacle);
        create_collision(state->scene, state->spirit, obstacle,
                         reset_user_handler, NULL, 0, NULL);
        asset_make_image_with_body(BRICK_PATH, obstacle);
      }

      void make_lava(state_t * state) {
        size_t len = LAVA_NUM[0];
        for (size_t i = 0; i < len; i++) {
          vector_t coord = (vector_t){LAVA1[i][0], LAVA1[i][1]};
          body_t *obstacle =
              make_obstacle(LAVA1[i][2], LAVA1[i][3], coord, "lava");
          scene_add_body(state->scene, obstacle);
          create_collision(state->scene, state->spirit, obstacle,
                           reset_user_handler, NULL, 0, NULL);
          asset_make_image_with_body(LAVA_PATH, obstacle);
        }
      }

      state_t *emscripten_init() {
        asset_cache_init();
        sdl_init(MIN, MAX);
        state_t *state = malloc(sizeof(state_t));
        state->points = 0;
        srand(time(NULL));
        state->scene = scene_init();
        state->pause_body = NULL;
        state->pause = false;

        // background image - the offset is a little strange
        SDL_Rect box =
            (SDL_Rect){.x = MIN.x + 125, .y = MIN.y, .w = MAX.x, .h = MAX.y};
        asset_make_image(BACKGROUND_PATH, box);

        body_t *spirit = make_spirit(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
        body_set_centroid(spirit, RESET_POS);
        state->spirit = spirit;
        scene_add_body(state->scene, spirit);

        // spirit
        asset_make_image_with_body(SPIRIT_FRONT_PATH, state->spirit);

        make_platforms(state);

        // make platform
        make_platforms(state, 1);
        // make lava
        make_lava(state);

        // make water
        sdl_on_key((key_handler_t)on_key);
        return state;
      }

      bool emscripten_main(state_t * state) {
        double dt = time_since_last_tick();
        player_wrap_edges(state);
        for (int i = 1; i < scene_bodies(state->scene); i++) {
          wrap_edges(scene_get_body(state->scene, i));
        }
        sdl_clear();
        sdl_render_scene(state->scene);
        list_t *body_assets = asset_get_asset_list();
        for (size_t i = 0; i < list_size(body_assets); i++) {
          asset_render(list_get(body_assets, i));
        }
        sdl_show();
        scene_tick(state->scene, dt);
        return false;
      }

      void emscripten_free(state_t * state) {
        list_free(asset_get_asset_list());
        scene_free(state->scene);
        asset_cache_destroy();
        free(state);
      }