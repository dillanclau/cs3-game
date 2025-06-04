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

const vector_t START_POS = {40, 30};
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
const size_t BRICK_NUM = 13;
size_t BRICKS1[BRICK_NUM][4] = {
                         {160, 425, 320, BRICK_WIDTH},
                         {560, 425, 150, BRICK_WIDTH},
                         {425, 300, 650, BRICK_WIDTH},
                         {325, 200, 650, BRICK_WIDTH},
                         {180, 75, 175, BRICK_WIDTH},
                         {500, 75, 175, BRICK_WIDTH},
                         {730, 330, 40, 60},
                         {30, 235, 60, 70},
                         {730, 90, 40, 60},
                         {715, 35, 70, 70},
                         {375, 0, 750, 30},
                         {0, 250, 30, 500},
                         {750, 250, 30, 500}
                        };

const size_t LAVA_WIDTH = 7;
const size_t LAVA_NUM[NUM_MAP] = {4, 0, 0};
size_t LAVA1[4][4] = {{180, 20, 165, LAVA_WIDTH},
                      {500, 90, 165, LAVA_WIDTH},
                      {500, 310, 100, LAVA_WIDTH},
                      {250, 310, 175, LAVA_WIDTH}};

const int16_t H_STEP = 50;
const int16_t V_STEP = 30;
const size_t ROWS = 50;

const size_t BODY_ASSETS = 2;

// velocity constants
const vector_t VELOCITY_LEFT = (vector_t){-200, 0};
const vector_t VELOCITY_RIGHT = (vector_t){200, 0};
const vector_t VELOCITY_UP = (vector_t){0, 200};

// gravity constants
const double GRAVITY = 200;

const char *SPIRIT_FRONT_PATH = "assets/waterspiritfront.png";
const char *BACKGROUND_PATH = "assets/dungeonbackground.png";
const char *PAUSE_PATH = "assets/pause.png";
const char *FONT_FILEPATH = "assets/Cascadia.ttf";
const char *BRICK_PATH = "assets/brick_texture.png";
const char *LAVA_PATH = "assets/lava.png";
const char *HOMEPAGE_PATH = "assets/homepage.png";

typedef enum {
  LEVEL1 = 1,
  LEVEL2 = 2,
  LEVEL3 = 3,
  HOMEPAGE = 4,
} screen_t;

typedef enum {
  NO_COLLISION = 0,
  RIGHT_COLLISION = 1,
  LEFT_COLLISION = 2,
  UP_COLLISION = 3,
  DOWN_COLLISION = 4,
} collision_type_t;

struct state {
  body_t *spirit;
  scene_t *scene;
  int16_t points;
  screen_t current_screen;
  collision_type_t collision_type;
  bool pause;
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

void reset_user(body_t *body) { body_set_centroid(body, START_POS); }

void reset_user_handler(body_t *body1, body_t *body2, vector_t axis, void *aux,
                        double force_const) {
  reset_user(body1);
}

// // handles the collisions between user and platform
// void platform_handler(body_t *body1, body_t *body2, vector_t axis, void *aux,
//                       double force_const) {
//   vector_t user_vel = body_get_velocity(body1);
//   vector_t user_pos = body_get_centroid(body1);
//   vector_t plat_pos = body_get_centroid(body2);
  
//   if ((user_vel.x > 0) && (plat_pos.x > user_pos.x)) {
//     user_vel.x = 0;
//   } else if ((user_vel.x < 0) && (plat_pos.x < user_pos.x)) {
//     user_vel.x = 0;
//   }

//   if ((user_vel.y > 0) && (plat_pos.y > user_pos.y)) {
//     user_vel.y = -user_vel.y;
//   } else if ((user_vel.y < 0) && (plat_pos.y < user_pos.y)) {
//     user_vel.y = 0;
//   }
//   body_set_velocity(body1, user_vel);
// }

// handles the collisions between user and platform
void platform_handler(body_t *body1, body_t *body2, vector_t axis, void *aux,
                      double force_const) {
  vector_t user_vel = body_get_velocity(body1);
  vector_t user_cen = body_get_centroid(body1);
  list_t *points = body_get_shape(body2);
  vector_t *v1 = list_get(points, 0);
  vector_t *v2 = list_get(points, 1);
  vector_t *v3 = list_get(points, 2);
  vector_t *v4 = list_get(points, 3);

  if ((user_cen.y > v1->y) && (user_cen.y < v4->y)) { // left or right
    user_vel.x = 0;
  } else if ((user_cen.x > v4->x) && (user_cen.x < v3->x) && (user_vel.y < 0)) { // up
    user_vel.y = 0;
  } else { // down
    user_vel.y = -user_vel.y;
  }
  body_set_velocity(body1, user_vel);
}

void make_level1(state_t *state) {
  // make brick platforms
  for (size_t i = 0; i < BRICK_NUM; i++) {
    vector_t coord = (vector_t){BRICKS1[i][0], BRICKS1[i][1]};
    if (BRICKS1[i][3] == 0) {
      BRICKS1[i][3] = BRICK_WIDTH;
    }
    body_t *obstacle =
        make_obstacle(BRICKS1[i][2], BRICKS1[i][3], coord, "platform");
    scene_add_body(state->scene, obstacle);
    create_collision(state->scene, state->spirit, obstacle, platform_handler,
                     NULL, 0, NULL);
    asset_make_image_with_body(BRICK_PATH, obstacle);
  }

  // make lava
  size_t lava_len = LAVA_NUM[0];
  for (size_t i = 0; i < lava_len; i++) {
    vector_t coord = (vector_t){LAVA1[i][0], LAVA1[i][1]};
    body_t *obstacle = make_obstacle(LAVA1[i][2], LAVA1[i][3], coord, "lava");
    scene_add_body(state->scene, obstacle);
    create_collision(state->scene, state->spirit, obstacle, reset_user_handler,
                     NULL, 0, NULL);
    asset_make_image_with_body(LAVA_PATH, obstacle);
  }
}

void go_to_level1(state_t *state) {
  asset_reset_asset_list();
  state->current_screen = LEVEL1;

  make_level1(state);
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
  asset_make_image(HOMEPAGE_PATH, box);
  // asset_make_text(FONT_FILEPATH,
  //                 (SDL_Rect){.x = 200, .y = 25, .w = 200, .h = 100},
  //                 "HOMEPAGE", TEXT_COLOR);
  // asset_make_text(FONT_FILEPATH,
  //                 (SDL_Rect){.x = 200, .y = 150, .w = 300, .h = 50},
  //                 "Press 1 to go to Level 1", TEXT_COLOR);
  // asset_make_text(FONT_FILEPATH,
  //                 (SDL_Rect){.x = 200, .y = 250, .w = 300, .h = 50},
  //                 "Press 2 to go to Level 2", TEXT_COLOR);
  // asset_make_text(FONT_FILEPATH,
  //                 (SDL_Rect){.x = 200, .y = 350, .w = 300, .h = 50},
  //                 "Press 3 to go to Level 3", TEXT_COLOR);
}

void pause(state_t *state) {
  state->pause = true;
  // list_t *body_list = list_init(4, free);
  // vector_t *v1 = malloc(sizeof(vector_t));
  // *v1 = (vector_t){150, 100};
  // list_add(body_list, v1);
  // vector_t *v2 = malloc(sizeof(vector_t));
  // *v2 = (vector_t){600, 400};
  // list_add(body_list, v2);
  // vector_t *v3 = malloc(sizeof(vector_t));
  // *v3 = (vector_t){150, 400};
  // list_add(body_list, v3);
  // vector_t *v4 = malloc(sizeof(vector_t));
  // *v4 = (vector_t){600, 100};
  // list_add(body_list, v4);
  // state->pause_body = body_init(body_list, 100, SPIRIT_COLOR);
  // body_set_centroid(state->pause_body, CENTER);
  // scene_add_body(state->scene, state->pause_body);
  // asset_make_image_with_body(PAUSE_FILEPATH, state->pause_body);
  asset_make_image(PAUSE_PATH,
                   (SDL_Rect){.x = 100, .y = 50, .w = 550, .h = 400});
}

void unpause(state_t *state) {
  // if (state->pause_body) {
  //   asset_remove_body(state->pause_body);
  //   body_remove(state->pause_body);
  //   body_free(state->pause_body);
  // }
  state->pause = false;
  list_t *asset_list = asset_get_asset_list();
  list_remove(asset_list, list_size(asset_list) - 1);
  list_remove(asset_list, list_size(asset_list) - 1);
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
  body_t *spirit = state->spirit;
  vector_t velocity = body_get_velocity(spirit);
  if (type == KEY_PRESSED) {
    switch (key) {
    case LEFT_ARROW:
      if (state->collision_type != RIGHT_COLLISION) {
        body_set_velocity(spirit, (vector_t){VELOCITY_LEFT.x, velocity.y});
      }
      break;
    case RIGHT_ARROW:
      if (state->collision_type != LEFT_COLLISION) {
        body_set_velocity(spirit, (vector_t){VELOCITY_RIGHT.x, velocity.y});
      }
      break;
    case UP_ARROW:
      if (state->collision_type != NO_COLLISION) {
        body_set_velocity(spirit, (vector_t){velocity.x, VELOCITY_UP.y});
      }
      break;
    case KEY_1:
      if (state->pause || state->current_screen == HOMEPAGE) {
        go_to_level1(state);
      }
      break;
    case KEY_2:
      if (state->pause || state->current_screen == HOMEPAGE) {
        go_to_level2(state);
      }
      break;
    case KEY_3:
      if (state->pause || state->current_screen == HOMEPAGE) {
        go_to_level3(state);
      }
      break;
    case KEY_H:
      if (state->pause) {
        go_to_homepage(state);
      }
      break;
    case KEY_P:
      if (state->current_screen == LEVEL1 || state->current_screen == LEVEL2 ||
          state->current_screen == LEVEL3) {
        pause(state);
      }
      break;
    case KEY_R:
      if (state->pause) {
        restart(state);
      }
      break;
    case KEY_U:
      if (state->pause) {
        unpause(state);
      }
      break;
    }
  } else {
    switch (key) {
    case LEFT_ARROW:
      body_set_velocity(spirit, (vector_t){0, velocity.y});
      break;
    case RIGHT_ARROW:
      body_set_velocity(spirit, (vector_t){0, velocity.y});
      break;
    }
  }
}

double rand_double(double low, double high) {
  return (high - low) * rand() / RAND_MAX + low;
}

void make_platforms(state_t *state, size_t idx) {
  for (size_t i = 0; i < BRICK_NUM; i++) {
    vector_t coord = (vector_t){BRICKS1[i][0], BRICKS1[i][1]};
    body_t *obstacle =
        make_obstacle(BRICKS1[i][2], BRICKS1[i][3], coord, "platform");
    scene_add_body(state->scene, obstacle);
    create_collision(state->scene, state->spirit, obstacle, platform_handler, NULL, 0, NULL);
    asset_make_image_with_body(BRICK_PATH, obstacle);
  }
}

  // // misc sized blocks
  // // {x, y, width, height}
  // const size_t NUM_OBST = 4;
  // int OBST[NUM_OBST][4] = {{730, 330, 40, 60},
  //                          {30, 235, 60, 70},
  //                          {730, 90, 40, 60},
  //                          {715, 35, 70, 70}};

  // for (size_t i = 0; i < NUM_OBST; i++) {
  //   vector_t coord = (vector_t){OBST[i][0], OBST[i][1]};
  //   body_t *obstacle = make_obstacle(OBST[i][2], OBST[i][3], coord, "obstacle");
  //   scene_add_body(state->scene, obstacle);
  //   create_collision(state->scene, state->spirit, obstacle, platform_handler,
  //                    NULL, 0, NULL);
  //   asset_make_image_with_body(BRICK_PATH, obstacle);
  // }

  // // boundaries
  // // {x, y, width, height}
  // const size_t BOUNDARIES = 3;
  // int EDGES[BOUNDARIES][4] = {
  //     {375, 0, 750, 30}, {0, 250, 30, 500}, {750, 250, 30, 500}};

  // for (size_t i = 0; i < BOUNDARIES; i++) {
  //   vector_t coord = (vector_t){EDGES[i][0], EDGES[i][1]};
  //   body_t *obstacle =
  //       make_obstacle(EDGES[i][2], EDGES[i][3], coord, "obstacle");
  //   scene_add_body(state->scene, obstacle);
  //   create_collision(state->scene, state->spirit, obstacle, platform_handler,
  //                    NULL, 0, NULL);
  //   asset_make_image_with_body(BRICK_PATH, obstacle);

void make_lava(state_t *state) {
  size_t len = LAVA_NUM[0];
  for (size_t i = 0; i < len; i++) {
    vector_t coord = (vector_t){LAVA1[i][0], LAVA1[i][1]};
    body_t *obstacle = make_obstacle(LAVA1[i][2], LAVA1[i][3], coord, "lava");
    scene_add_body(state->scene, obstacle);
    create_collision(state->scene, state->spirit, obstacle, reset_user_handler,
                     NULL, 0, NULL);
    asset_make_image_with_body(LAVA_PATH, obstacle);
  }
}

collision_type_t collision(state_t *state) {
  body_t *spirit = state->spirit;
  scene_t *scene = state->scene;
  for (size_t i = 1; i < scene_bodies(scene); i++) {
    body_t *platform = scene_get_body(scene, i);
    if (find_collision(spirit, platform).collided) {
      vector_t spirit_vel = body_get_velocity(spirit);
      vector_t spirit_cen = body_get_centroid(spirit);
      list_t *points = body_get_shape(platform);
      vector_t *v1 = list_get(points, 0);
      vector_t *v2 = list_get(points, 1);
      vector_t *v3 = list_get(points, 2);
      vector_t *v4 = list_get(points, 3);
      if ((spirit_cen.y > v1->y) && (spirit_cen.y < v4->y) && (spirit_vel.x >= 0)) {
        return LEFT_COLLISION;
      } else if ((spirit_cen.y > v1->y) && (spirit_cen.y < v4->y) && (spirit_vel.x <= 0)) {
        return RIGHT_COLLISION;
      } else if ((spirit_cen.x > v4->x) && (spirit_cen.x < v3->x) && (spirit_vel.y <= 0)) { 
        return UP_COLLISION;
      } else { 
        return DOWN_COLLISION;
      }
    }
  }
  return NO_COLLISION;
}

state_t *emscripten_init() {
  asset_cache_init();
  sdl_init(MIN, MAX);
  state_t *state = malloc(sizeof(state_t));
  state->points = 0;
  srand(time(NULL));
  state->scene = scene_init();
  state->current_screen = LEVEL1;
  state->pause = false;
  state->collision_type = UP_COLLISION;

  SDL_Rect box = (SDL_Rect){.x = MIN.x, .y = MIN.y, .w = MAX.x, .h = MAX.y};
  asset_make_image(BACKGROUND_PATH, box);

  body_t *spirit = make_spirit(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
  body_set_centroid(spirit, START_POS);
  state->spirit = spirit;
  scene_add_body(state->scene, spirit);

  // spirit
  asset_make_image_with_body(SPIRIT_FRONT_PATH, state->spirit);

  // make platform
  make_platforms(state, 1);

  // make lava
  make_lava(state);

  // make water
  sdl_on_key((key_handler_t)on_key);

  return state;
}

bool emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  sdl_clear();
  sdl_render_scene(state->scene);
  list_t *body_assets = asset_get_asset_list();
  for (size_t i = 0; i < list_size(body_assets); i++) {
    asset_render(list_get(body_assets, i));
  }

  state->collision_type = collision(state);

  // apply gravity
  body_t *spirit = state->spirit;
  vector_t spirit_velocity = body_get_velocity(spirit);
  if (state->collision_type != UP_COLLISION) {
    body_set_velocity(spirit, (vector_t){spirit_velocity.x, spirit_velocity.y - (GRAVITY * dt)});
  }

  sdl_show();
  if (!state->pause) {
    scene_tick(state->scene, dt);
  }
  return false;
}

void emscripten_free(state_t *state) {
  list_free(asset_get_asset_list());
  scene_free(state->scene);
  asset_cache_destroy();
  free(state);
}
