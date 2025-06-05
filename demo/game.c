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

const vector_t START_POS = {40, 40};
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
const size_t BRICK_NUM[NUM_MAP] = {14, 11, 12};

const size_t NUMBER_OF_LEVELS = 3;

// point range thresholds
size_t RED_THRESHOLD = 25;
size_t YELLOW_THRESHOLD = 50;
size_t GREEN_THRESHOLD = 75;

// x, y, w, h
// Bricks for Map 1
size_t BRICKS1[14][4] = {{375, -500, 750, 30},
                         {160, 380, 320, BRICK_WIDTH},
                         {560, 380, 150, BRICK_WIDTH},
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
                         {750, 250, 30, 500}};

// Bricks for Map 2
size_t BRICKS2[11][4] = {{100, 390, 200, BRICK_WIDTH}, // where the door is
                         {450, 385, 300, BRICK_WIDTH},
                         {350, 290, 350, BRICK_WIDTH}, // next row
                         {630, 270, 300, BRICK_WIDTH},
                         {225, 200, 450, BRICK_WIDTH}, // third row
                         {500, 130, 300, BRICK_WIDTH},
                         {100, 80, 200, BRICK_WIDTH}, // starting platform
                         {710, 30, 80, 60},           // misc sq
                         {375, 0, 750, 30},           // border
                         {0, 250, 30, 500},
                         {750, 250, 30, 500}};

size_t BRICKS3[12][4] = {{50, 390, 100, BRICK_WIDTH},  // where the door is
                         {185, 275, BRICK_WIDTH, 200}, // left column
                         {375, 250, BRICK_WIDTH, 250}, // second column
                         {435, 315, 120, BRICK_WIDTH},
                         {580, 230, 90, BRICK_WIDTH},
                         {690, 370, 120, BRICK_WIDTH},
                         {240, 230, 90, BRICK_WIDTH},
                         {140, 325, 90, BRICK_WIDTH},
                         {325, 120, 650, BRICK_WIDTH}, // starting platform
                         {375, 0, 750, 30},            // border
                         {0, 250, 30, 500},
                         {750, 250, 30, 500}};

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
const double GRAVITY = 250;

// assets
const char *SPIRIT_FRONT_PATH = "assets/waterspiritfront.png";
const char *SPIRIT_RIGHT_PATH = "assets/waterspiritright.png";
const char *SPIRIT_LEFT_PATH = "assets/waterspiritleft.png";
const char *BACKGROUND_PATH = "assets/dungeonbackground.png";
const char *PAUSE_PATH = "assets/pause.png";
const char *FONT_FILEPATH = "assets/Cascadia.ttf";
const char *BRICK_PATH = "assets/brick_texture.png";
const char *LAVA_PATH = "assets/lava.png";
const char *HOMEPAGE_PATH = "assets/homepage.png";
const char *ELEVATOR_PATH = "assets/elevator.png";
const char *DOOR_PATH = "assets/door.png";
const char *GEM_PATH = "assets/gem.png";
// const char *RED_GEM_PATH = ;
// const char *YELLOW_GEM_PATH = ;
// const char *GREEN_GEM_PATH = ;

typedef enum {
  LEVEL1 = 1,
  LEVEL2 = 2,
  LEVEL3 = 3,
  HOMEPAGE = 4,
} screen_t;

struct state {
  body_t *spirit;
  scene_t *scene;
  int16_t points;
  screen_t current_screen;
  bool collided;
  bool pause;
  size_t level_points[3];
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
  body_t *obstacle = body_init_with_info(c, 1e6, OBS_COLOR, info, NULL);
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

body_t *make_gem(double outer_radius, double inner_radius, vector_t center) {
  center.y += inner_radius;
  list_t *c = list_init(SPIRIT_NUM_POINTS, free);
  for (size_t i = 0; i < SPIRIT_NUM_POINTS; i++) {
    double angle = 2 * M_PI * i / SPIRIT_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + inner_radius * cos(angle),
                    center.y + outer_radius * sin(angle)};
    list_add(c, v);
  }
  body_t *gem = body_init_with_info(c, 1, OBS_COLOR, "gem", NULL);
  return gem;
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

void reset_user(body_t *body) { body_set_centroid(body, START_POS); }

void reset_user_handler(body_t *body1, body_t *body2, vector_t axis, void *aux,
                        double force_const) {
  reset_user(body1);
  asset_make_image(GAME_OVER_PATH, (SDL_Rect){.x = 100, .y = 50, .w = 550, .h = 400});
  if (state->current_screen == LEVEL1) {
    state->level_points[0] = 0;
  }
  else if (state->current_screen == LEVEL2) {
    state->level_points[1] = 0;
  }
  else if (state->current_screen == LEVEL3) {
    state->level_points[2] = 0;
  }
}

// handles the collisions between user and platform
void platform_handler(body_t *body1, body_t *body2, vector_t axis, void *aux,
                      double force_const) {
  vector_t user_vel = body_get_velocity(body1);
  vector_t user_pos = body_get_centroid(body1);
  vector_t plat_pos = body_get_centroid(body2);
  if ((user_vel.x > 0) && (plat_pos.x > user_pos.x)) {
    user_vel.x = 0;
  } else if ((user_vel.x < 0) && (plat_pos.x < user_pos.x)) {
    user_vel.x = 0;
  }

  if ((user_vel.y > 0) && (plat_pos.y > user_pos.y)) {
    user_vel.y = -user_vel.y;
  } else if ((user_vel.y < 0) && (plat_pos.y < user_pos.y)) {
    user_vel.y = 0;
  }
  body_set_velocity(body1, user_vel);
}

void make_level1(state_t *state) {
  // make brick platforms
  size_t brick_len = BRICK_NUM[0];
  for (size_t i = 0; i < brick_len; i++) {
    vector_t coord = (vector_t){BRICKS1[i][0], BRICKS1[i][1]};
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

  // make gem
  vector_t center = (vector_t) {.x = 100, .y = 100};
  body_t *gem = make_gem(OUTER_RADIUS, INNER_RADIUS, center);
  scene_add_body(state->scene, gem);
  // create_destructive_collision(state->scene, state->spirit, gem);
  // the destructive collision is strange for some reason
  create_collision(state->scene, state->spirit, gem, reset_user_handler,
                     NULL, 0, NULL);
  asset_make_image_with_body(GEM_PATH, gem);
}

void make_level2(state_t *state) {
  size_t brick_len = BRICK_NUM[1];
  for (size_t i = 0; i < brick_len; i++) {
    vector_t coord = (vector_t){BRICKS2[i][0], BRICKS2[i][1]};
    body_t *obstacle =
        make_obstacle(BRICKS2[i][2], BRICKS2[i][3], coord, "platform");
    scene_add_body(state->scene, obstacle);
    create_collision(state->scene, state->spirit, obstacle, platform_handler,
                     NULL, 0, NULL);
    asset_make_image_with_body(BRICK_PATH, obstacle);
  }
}

void make_level3(state_t *state) {
  size_t brick_len = BRICK_NUM[2];
  for (size_t i = 0; i < brick_len; i++) {
    vector_t coord = (vector_t){BRICKS3[i][0], BRICKS3[i][1]};
    body_t *obstacle =
        make_obstacle(BRICKS3[i][2], BRICKS3[i][3], coord, "platform");
    scene_add_body(state->scene, obstacle);
    create_collision(state->scene, state->spirit, obstacle, platform_handler,
                     NULL, 0, NULL);
    asset_make_image_with_body(BRICK_PATH, obstacle);
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
  asset_make_text(FONT_FILEPATH,
                  (SDL_Rect){.x = 200, .y = 25, .w = 200, .h = 100},
                  "HOMEPAGE", TEXT_COLOR);
  asset_make_text(FONT_FILEPATH,
                  (SDL_Rect){.x = 200, .y = 150, .w = 300, .h = 50},
                  "Press 1 to go to Level 1", TEXT_COLOR);
  asset_make_text(FONT_FILEPATH,
                  (SDL_Rect){.x = 200, .y = 250, .w = 300, .h = 50},
                  "Press 2 to go to Level 2", TEXT_COLOR);
  asset_make_text(FONT_FILEPATH,
                  (SDL_Rect){.x = 200, .y = 350, .w = 300, .h = 50},
                  "Press 3 to go to Level 3", TEXT_COLOR);
      // SDL_Rect level_gem_box[3] = [
      //   (SDL_Rect){.x = 100, .y = 500, .w = 50, .h = 50},
      //   (SDL_Rect){.x = 200, .y = 500, .w = 50, .h = 50},
      //   (SDL_Rect){.x = 300, .y = 500, .w = 50, .h = 50}
      // ];
      // for (size_t i = 0; i < NUMBER_OF_LEVELS; i++) {
      //   if (state->level_points[i] > GREEN_THRESHOLD) {
      //     asset_make_image(GREEN_GEM_PATH, level_gem_box[i]);
      //     break;
      //   } else if (state->level_points[i] > YELLOW_THRESHOLD) {
      //     asset_make_image(YELLOW_GEM_PATH, level_gem_box[i]);
      //     break;
      //   } else if (state->level_points[i] > RED_THRESHOLD) {
      //     asset_make_image(RED_GEM_PATH, level_gem_box[i]);
      //     break;
      //   }
      // }
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
  body_t *spirit = scene_get_body(state->scene, 0);
  vector_t velocity = body_get_velocity(spirit);
  if (type == KEY_PRESSED) {
    switch (key) {
    case LEFT_ARROW:
      body_set_velocity(spirit, (vector_t){VELOCITY_LEFT.x, velocity.y});
      // NOT SURE HOW TO CHANGE THE TEXTURE
      // asset_change_texture(SPIRIT_LEFT_PATH, 1);
      break;
    case RIGHT_ARROW:
      body_set_velocity(spirit, (vector_t){VELOCITY_RIGHT.x, velocity.y});
      // asset_change_texture(SPIRIT_RIGHT_PATH, 1);
      break;
    case UP_ARROW:
      if (state->collided) {
        body_set_velocity(spirit, (vector_t){velocity.x, VELOCITY_UP.y});
        break;
      }
      // asset_change_texture(SPIRIT_FRONT_PATH, 1);
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
  size_t len = BRICK_NUM[0];
  for (size_t i = 0; i < len; i++) {
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

  // misc sized blocks
  // {x, y, width, height}
  const size_t NUM_OBST = 4;
  int OBST[NUM_OBST][4] = {{730, 330, 40, 60},
                           {30, 235, 60, 70},
                           {730, 90, 40, 60},
                           {715, 35, 70, 70}};

  for (size_t i = 0; i < NUM_OBST; i++) {
    vector_t coord = (vector_t){OBST[i][0], OBST[i][1]};
    body_t *obstacle = make_obstacle(OBST[i][2], OBST[i][3], coord, "obstacle");
    scene_add_body(state->scene, obstacle);
    create_collision(state->scene, state->spirit, obstacle, platform_handler,
                     NULL, 0, NULL);
    asset_make_image_with_body(BRICK_PATH, obstacle);
  }

  // boundaries
  // {x, y, width, height}
  const size_t BOUNDARIES = 3;
  int EDGES[BOUNDARIES][4] = {
      {375, 0, 750, 30}, {0, 250, 30, 500}, {750, 250, 30, 500}};

  for (size_t i = 0; i < BOUNDARIES; i++) {
    vector_t coord = (vector_t){EDGES[i][0], EDGES[i][1]};
    body_t *obstacle =
        make_obstacle(EDGES[i][2], EDGES[i][3], coord, "obstacle");
    scene_add_body(state->scene, obstacle);
    create_collision(state->scene, state->spirit, obstacle, platform_handler,
                     NULL, 0, NULL);
    asset_make_image_with_body(BRICK_PATH, obstacle);
  }
}

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

bool collision(state_t *state) {
  body_t *spirit = state->spirit;
  scene_t *scene = state->scene;
  for (size_t i = 1; i < scene_bodies(scene); i++) {
    if (find_collision(spirit, scene_get_body(scene, i)).collided) {
      return true;
    }
  }
  return false;
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
      // state->level_points[0] = 0; // for level 1
      // state->level_points[1] = 0; // for level 2
      // state->level_points[2] = 0; // for level 3
  state->collided = false;

  SDL_Rect box = (SDL_Rect){.x = MIN.x, .y = MIN.y, .w = MAX.x, .h = MAX.y};
  asset_make_image(BACKGROUND_PATH, box);

  body_t *spirit = make_spirit(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
  body_set_centroid(spirit, START_POS);
  state->spirit = spirit;
  scene_add_body(state->scene, spirit);

  // spirit
  asset_make_image_with_body(SPIRIT_FRONT_PATH, state->spirit);

  // make level
  make_level1(state);
  // make_level3(state);

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

  // apply gravity
  state->collided = collision(state);
  body_t *spirit = state->spirit;
  vector_t spirit_velocity = body_get_velocity(spirit);
  if (!(state->collided)) {
    body_set_velocity(spirit, (vector_t){spirit_velocity.x,
                                         spirit_velocity.y - (GRAVITY * dt)});
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
