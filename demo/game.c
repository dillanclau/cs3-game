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

// make the spirit
const double OUTER_RADIUS = 15;
const double INNER_RADIUS = 15;
const size_t SPIRIT_NUM_POINTS = 20;

// colors
const color_t OBS_COLOR = (color_t){0.2, 0.2, 0.3};
const color_t SPIRIT_COLOR = (color_t){0.1, 0.9, 0.2};
const color_t CLOCK_COL = {1, 1, .5};

// constants to create platforms
const size_t NUM_MAP = 3;
const size_t BRICK_WIDTH = 20;
const size_t BRICK_NUM[NUM_MAP] = {14, 12, 12};

const size_t NUMBER_OF_LEVELS = 3;

// point range thresholds
size_t RED_THRESHOLD = 0;
size_t ORANGE_THRESHOLD = 5;
size_t GREEN_THRESHOLD = 15;

// x, y, w, h
// Bricks for Map 1
size_t BRICKS1[14][4] = {{375, -500, 750, 30},
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
                         {750, 250, 30, 500}};

// Bricks for Map 2
const size_t BRICKS2[12][4] = {
    {100, 390, 200, 20},                      // where the door is
    {450, 390, 300, 20}, {350, 290, 350, 20}, // next row
    {630, 270, 300, 20}, {715, 290, 70, 60},  {225, 200, 450, 20}, // third row
    {500, 130, 300, 20}, {100, 80, 200, 20}, // starting platform
    {710, 30, 80, 60},                       // misc sq
    {375, 0, 750, 30},                       // border
    {0, 250, 30, 500},   {750, 250, 30, 500}};

// get rid of brick_width and hard code the whole thing
const size_t BRICKS3[12][4] = {
    {50, 390, 100, 20},  // where the door is
    {185, 275, 20, 200}, // left column
    {375, 250, 20, 250}, // second column
    {435, 315, 120, 20}, {580, 230, 90, 20},
    {690, 370, 120, 20}, {240, 230, 90, 20},
    {140, 325, 90, 20},  {325, 120, 650, 20}, // starting platform
    {375, 0, 750, 30},                        // border
    {0, 250, 30, 500},   {750, 250, 30, 500}};

// width of the lava + water = 11
const size_t LAVA_NUM[NUM_MAP] = {4, 4, 2};
const size_t LAVA1[4][4] = {{180, 15, 165, 11},
                            {500, 85, 165, 11},
                            {500, 310, 100, 11},
                            {252, 310, 140, 11}};

const size_t LAVA2[4][4] = {{500, 15, 140, 11},
                            {580, 140, 80, 11},
                            {510, 400, 60, 11},
                            {390, 400, 60, 11}};

const size_t LAVA3[2][4] = {{550, 15, 90, 11}, {225, 240, 50, 11}};

const size_t WATER_NUM[NUM_MAP] = {2, 2, 2};
const size_t WATER1[2][4] = {{500, 210, 165, 11}, {220, 210, 120, 11}};

const size_t WATER2[2][4] = {{300, 300, 120, 11}, {110, 90, 100, 11}};

const size_t WATER3[2][4] = {{280, 15, 160, 11}, {670, 380, 70, 11}};

// exits
size_t EXITS[3][4] = {{60, 458, INNER_RADIUS * 3, OUTER_RADIUS * 3},
                      {60, 424, INNER_RADIUS * 3, OUTER_RADIUS * 3},
                      {60, 424, INNER_RADIUS * 3, OUTER_RADIUS * 3}};

// elevators
const size_t ELEVATORS[3][4] = {
    {50, 220, 70, 20}, {700, 25, 70, 20}, {50, 200, 70, 20}};
// elevator buttons
const size_t E_BUTTONS[2][4] = {{475, 150, 30, 20}, {400, 25, 30, 20}};

// doors
const size_t DOORS[2][4] = {{300, 245, 30, 70}, {250, 175, 30, 90}};
// doors buttons
const size_t BUTTONS[2][4] = {{40, 100, 30, 20}, {500, 140, 30, 20}};

// gem constants
const size_t GEM_NUM[3] = {3, 3, 3};
const size_t GEM1[3][2] = {{180, 100}, {560, 450}, {375, 325}};
const size_t GEM2[3][2] = {{120, 100}, {430, 310}, {450, 410}};
const size_t GEM3[3][2] = {{670, 390}, {580, 250}, {135, 345}};

// clock constants
const vector_t CLOCK_POS = {.x = 375, .y = 10};
const size_t TEXT_SIZE = 14;
const size_t TEXT_HEIGHT_SCALE = 2;

// velocity constants
const vector_t VELOCITY_LEFT = (vector_t){-200, 0};
const vector_t VELOCITY_RIGHT = (vector_t){200, 0};
const vector_t VELOCITY_UP = (vector_t){0, 240};

// gravity constants
const double GRAVITY = 320;

// assets
const char *SPIRIT_FRONT_PATH = "assets/waterspiritfront.png";
const char *SPIRIT_RIGHT_PATH = "assets/waterspiritright.png";
const char *SPIRIT_LEFT_PATH = "assets/waterspiritleft.png";
const char *BACKGROUND_PATH = "assets/dungeonbackground.png";
const char *PAUSE_PATH = "assets/pause.png";
const char *FONT_FILEPATH = "assets/Cascadia.ttf";
const char *BRICK_PATH = "assets/brick_texture.png";
const char *HOMEPAGE_PATH = "assets/homepage.png";
const char *ELEVATOR_PATH = "assets/elevator.png";
const char *DOOR_PATH = "assets/door.png";
const char *GEM_PATH = "assets/gem.png";
const char *RED_GEM_PATH = "assets/red_gem.png";
const char *ORANGE_GEM_PATH = "assets/orange_gem.png";
const char *GREEN_GEM_PATH = "assets/green_gem.png";
const char *EXIT_DOOR_PATH = "assets/exit_door.png";
const char *LAVA1_PATH = "assets/lavaframe1.png";
const char *LAVA2_PATH = "assets/lavaframe2.png";
const char *LAVA3_PATH = "assets/lavaframe3.png";
const char *WATER1_PATH = "assets/waterframe1.png";
const char *WATER2_PATH = "assets/waterframe2.png";
const char *WATER3_PATH = "assets/waterframe3.png";
const char *GAME_OVER_PATH = "assets/game_over.png";
const char *WIN_PATH = "assets/level_completed.png";

const char *BACKGROUND_MUSIC_PATH = "assets/background_music.mp3";
const char *GEM_SOUND_PATH = "assets/gem_sound.mp3";
const char *COMPLETED_SOUND_PATH = "assets/level_completed_sound.mp3";
const char *FAILED_SOUND_PATH = "assets/level_failed_sound.mp3";
const char *JUMP_SOUND_PATH = "assets/jump_sound.mp3";

const char *DOOR_BUTTON_UNPRESSED_PATH = "assets/button_unpressed.png";
const char *DOOR_BUTTON_PRESSED_PATH = "assets/button_pressed.png";
const char *ELEVATOR_BUTTON_UNPRESSED_PATH =
    "assets/elevator_button_unpressed.png";
const char *ELEVATOR_BUTTON_PRESSED_PATH = "assets/elevator_button_pressed.png";

typedef enum {
  LEVEL1 = 1,
  LEVEL2 = 2,
  LEVEL3 = 3,
  HOMEPAGE = 4,
} screen_t;

struct state {
  body_t *spirit;
  scene_t *scene;
  screen_t current_screen;
  collision_type_t collision_type;
  bool pause;
  bool elevator;
  double level_points[3];
  bool level_completed[3];
  double time;
  bool music_played;
  TTF_Font *font;
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

void move_elevator(state_t *state) {
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *spirit = state->spirit;
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), "elevator") == 0) {
      vector_t centroid = body_get_centroid(body);

      if (state->current_screen == LEVEL2) {
        if (centroid.y + 10 > 320) {
          body_set_velocity(body, (vector_t){0, -20});
        } else if (centroid.y - 10 < 220) {
          body_set_velocity(body, (vector_t){0, 20});
        }
      }

      if (state->current_screen == LEVEL3) {
        if (centroid.x == 700) { // first elevator
          if (centroid.y + 10 > 320) {
            body_set_velocity(body, (vector_t){0, -20});
          } else if (centroid.y - 10 < 25) {
            body_set_velocity(body, (vector_t){0, 20});
          }
        } else if (centroid.x == 50) { // second elevator
          if (centroid.y + 10 > 320) {
            body_set_velocity(body, (vector_t){0, -20});
          } else if (centroid.y - 10 < 200) {
            body_set_velocity(body, (vector_t){0, 20});
          }
        }
      }

      if (find_collision(body, spirit).collided &&
          (state->collision_type == UP_COLLISION ||
           state->collision_type == UP_LEFT_COLLISION ||
           state->collision_type == UP_RIGHT_COLLISION)) {
        vector_t spirit_vel = body_get_velocity(spirit);
        vector_t elevator_vel = body_get_velocity(body);
        if (spirit_vel.y <= elevator_vel.y) {
          body_set_velocity(spirit, (vector_t){body_get_velocity(spirit).x,
                                               body_get_velocity(body).y});
        }
      }
    }
  }
}

// Handlers
void reset_user(body_t *body) {
  body_set_centroid(body, (vector_t){-500, -500});
}

void reset_user_handler(body_t *body1, body_t *body2, vector_t axis, void *aux,
                        double force_const) {
  reset_user(body1);
  asset_make_image(GAME_OVER_PATH,
                   (SDL_Rect){.x = 100, .y = 50, .w = 550, .h = 400});
  sdl_play_level_failed(FAILED_SOUND_PATH);
}

void win_handler(body_t *body1, body_t *body2, vector_t axis, void *aux,
                 double force_const) {
  reset_user(body1);
  asset_make_image(WIN_PATH, (SDL_Rect){.x = 100, .y = 50, .w = 550, .h = 400});
  sdl_play_level_completed(COMPLETED_SOUND_PATH);
}

void elevator_user_handler(body_t *body1, body_t *body2, vector_t axis,
                           void *aux, double force_const) {
  vector_t vel = body_get_velocity(body1);
  vector_t cen = body_get_centroid(body1);
  list_t *pts = body_get_shape(body2);

  vector_t *v1 = list_get(pts, 0);
  vector_t *v2 = list_get(pts, 1);
  vector_t *v3 = list_get(pts, 2);
  vector_t *v4 = list_get(pts, 3);

  // vector_t user_vel = body_get_velocity(body1);
  // vector_t user_pos = body_get_centroid(body1);
  vector_t plat_vel = body_get_velocity(body2);
  vector_t plat_pos = body_get_centroid(body2);

  if (cen.x > v4->x - INNER_RADIUS && cen.x < v3->x + INNER_RADIUS &&
      cen.y - (INNER_RADIUS - 8) >= v4->y) {
    body_set_centroid(body1, (vector_t){cen.x, plat_pos.y + BRICK_WIDTH + 3});
    vel.y = plat_vel.y;
  }

  if (cen.x > v1->x - INNER_RADIUS && cen.x < v2->x + INNER_RADIUS &&
      cen.y < v1->y) {
    vel.y = -vel.y;
  }

  if (cen.y > v1->y - OUTER_RADIUS && cen.y < v4->y + OUTER_RADIUS &&
      cen.x < v1->x) {
    vel.x = 0;
  }

  if (cen.y > v2->y - OUTER_RADIUS && cen.y < v3->y + OUTER_RADIUS &&
      cen.x > v2->x) {
    vel.x = 0;
  }
  body_set_velocity(body1, vel);
}

void gem_user_handler(body_t *body1, body_t *body2, vector_t axis, void *aux,
                      double force_const) {
  // reset_user(body1);
  // do the points and stuff
  asset_remove_body(body2);
  body_remove(body2);
  sdl_play_gem_sound(GEM_SOUND_PATH);
}

void platform_handler(body_t *body1, body_t *body2, vector_t axis, void *aux,
                      double force_const) {

  vector_t vel = body_get_velocity(body1);
  vector_t cen = body_get_centroid(body1);
  list_t *pts = body_get_shape(body2);
  vector_t *v1 = list_get(pts, 0);
  vector_t *v2 = list_get(pts, 1);
  vector_t *v3 = list_get(pts, 2);
  vector_t *v4 = list_get(pts, 3);

  if (cen.x > v4->x - INNER_RADIUS && cen.x < v3->x + INNER_RADIUS &&
      cen.y - (INNER_RADIUS - 8) >= v4->y) {
    vel.y = 0;
  }

  if (cen.x > v1->x - INNER_RADIUS && cen.x < v2->x + INNER_RADIUS &&
      cen.y < v1->y) {
    vel.y = -vel.y;
  }

  if (cen.y > v1->y - OUTER_RADIUS && cen.y < v4->y + OUTER_RADIUS &&
      cen.x < v1->x) {
    vel.x = 0;
  }

  if (cen.y > v2->y - OUTER_RADIUS && cen.y < v3->y + OUTER_RADIUS &&
      cen.x > v2->x) {
    vel.x = 0;
  }

  body_set_velocity(body1, vel);
}

vector_t get_dimensions_for_text(char *text) {
  return (vector_t){strlen(text) * TEXT_SIZE, TEXT_SIZE * TEXT_HEIGHT_SCALE};
}

void init_bgd_player(state_t *state) {
  state->time = 0;
  SDL_Rect box = (SDL_Rect){.x = MIN.x, .y = MIN.y, .w = MAX.x, .h = MAX.y};
  asset_make_image(BACKGROUND_PATH, box);

  body_t *spirit = make_spirit(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
  body_set_centroid(spirit, START_POS);
  state->spirit = spirit;
  state->collision_type = NO_COLLISION;
  scene_add_body(state->scene, spirit);

  // spirit
  asset_make_spirit(SPIRIT_FRONT_PATH, SPIRIT_LEFT_PATH, SPIRIT_RIGHT_PATH,
                    spirit);
}

void make_level1(state_t *state) {
  state->current_screen == LEVEL1;
  init_bgd_player(state);

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
    asset_make_anim(LAVA1_PATH, LAVA2_PATH, LAVA3_PATH, obstacle);
  }

  // make water
  size_t water_len = WATER_NUM[0];
  for (size_t i = 0; i < water_len; i++) {
    vector_t coord = (vector_t){WATER1[i][0], WATER1[i][1]};
    body_t *obstacle =
        make_obstacle(WATER1[i][2], WATER1[i][3], coord, "water");
    scene_add_body(state->scene, obstacle);
    asset_make_anim(WATER1_PATH, WATER2_PATH, WATER3_PATH, obstacle);
  }

  // make gem
  size_t gem_len = GEM_NUM[0];
  for (size_t i = 0; i < gem_len; i++) {
    vector_t center = (vector_t){GEM1[i][0], GEM1[i][1]};
    body_t *gem = make_gem(OUTER_RADIUS, INNER_RADIUS, center);
    scene_add_body(state->scene, gem);
    create_collision(state->scene, state->spirit, gem, gem_user_handler, NULL,
                     0, NULL);
    asset_make_image_with_body(GEM_PATH, gem);
  }

  // make exit
  vector_t coord = (vector_t){EXITS[0][0], EXITS[0][1]};
  body_t *exit = make_obstacle(EXITS[0][2], EXITS[0][3], coord, "exit");
  scene_add_body(state->scene, exit);
  create_collision(state->scene, state->spirit, exit, win_handler, NULL, 0,
                   NULL);
  asset_make_image_with_body(EXIT_DOOR_PATH, exit);

  // make_clock(state);
}

void make_level2(state_t *state) {
  state->current_screen == LEVEL2;
  init_bgd_player(state);

  // make brick platforms
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

  // make lava
  size_t lava_len = LAVA_NUM[1];
  for (size_t i = 0; i < lava_len; i++) {
    vector_t coord = (vector_t){LAVA2[i][0], LAVA2[i][1]};
    body_t *obstacle = make_obstacle(LAVA2[i][2], LAVA2[i][3], coord, "lava");
    scene_add_body(state->scene, obstacle);
    create_collision(state->scene, state->spirit, obstacle, reset_user_handler,
                     NULL, 0, NULL);
    asset_make_anim(LAVA1_PATH, LAVA2_PATH, LAVA3_PATH, obstacle);
  }

  // make water
  size_t water_len = WATER_NUM[1];
  for (size_t i = 0; i < water_len; i++) {
    vector_t coord = (vector_t){WATER2[i][0], WATER2[i][1]};
    body_t *obstacle =
        make_obstacle(WATER2[i][2], WATER2[i][3], coord, "water");
    scene_add_body(state->scene, obstacle);
    asset_make_anim(WATER1_PATH, WATER2_PATH, WATER3_PATH, obstacle);
  }

  // make gem
  size_t gem_len = GEM_NUM[1];
  for (size_t i = 0; i < gem_len; i++) {
    vector_t center = (vector_t){GEM2[i][0], GEM2[i][1]};
    body_t *gem = make_gem(OUTER_RADIUS, INNER_RADIUS, center);
    scene_add_body(state->scene, gem);
    create_collision(state->scene, state->spirit, gem, gem_user_handler, NULL,
                     0, NULL);
    asset_make_image_with_body(GEM_PATH, gem);
  }

  // make exit
  vector_t coord = (vector_t){EXITS[1][0], EXITS[1][1]};
  body_t *exit = make_obstacle(EXITS[1][2], EXITS[1][3], coord, "exit");
  scene_add_body(state->scene, exit);
  create_collision(state->scene, state->spirit, exit, win_handler, NULL, 0,
                   NULL);
  asset_make_image_with_body(EXIT_DOOR_PATH, exit);

  // make elevator
  vector_t e_coord = (vector_t){ELEVATORS[0][0], ELEVATORS[0][1]};
  body_t *elevator =
      make_obstacle(ELEVATORS[0][2], ELEVATORS[0][3], e_coord, "elevator");
  scene_add_body(state->scene, elevator);
  create_collision(state->scene, state->spirit, elevator, platform_handler,
                   NULL, 0, NULL);
  asset_make_image_with_body(ELEVATOR_PATH, elevator);

  // make elevator button
  vector_t e_button_coord = (vector_t){E_BUTTONS[0][0], E_BUTTONS[0][1]};
  body_t *e_button = make_obstacle(E_BUTTONS[0][2], E_BUTTONS[0][3],
                                   e_button_coord, "elevator button");
  scene_add_body(state->scene, e_button);
  create_collision(state->scene, state->spirit, e_button, platform_handler,
                   NULL, 0, NULL);
  asset_make_button(ELEVATOR_BUTTON_UNPRESSED_PATH,
                    ELEVATOR_BUTTON_PRESSED_PATH, e_button);

  // make door
  vector_t door_coord = (vector_t){DOORS[0][0], DOORS[0][1]};
  body_t *door = make_obstacle(DOORS[0][2], DOORS[0][3], door_coord, "door");
  scene_add_body(state->scene, door);
  create_collision(state->scene, state->spirit, door, platform_handler, NULL, 0,
                   NULL);
  asset_make_image_with_body(DOOR_PATH, door);

  // make door button
  vector_t button_coord = (vector_t){BUTTONS[0][0], BUTTONS[0][1]};
  body_t *button =
      make_obstacle(BUTTONS[0][2], BUTTONS[0][3], button_coord, "door button");
  scene_add_body(state->scene, button);
  create_collision(state->scene, state->spirit, button, platform_handler, NULL,
                   0, NULL);
  asset_make_button(DOOR_BUTTON_UNPRESSED_PATH, DOOR_BUTTON_PRESSED_PATH,
                    button);

  // make_clock(state);
}

void make_level3(state_t *state) {
  state->current_screen == LEVEL3;
  init_bgd_player(state);

  for (size_t i = 1; i < 3; i++) {
    vector_t elevator_coord = (vector_t){ELEVATORS[i][0], ELEVATORS[i][1]};
    body_t *obstacle = make_obstacle(ELEVATORS[i][2], ELEVATORS[i][3],
                                     elevator_coord, "elevator");
    scene_add_body(state->scene, obstacle);
    create_collision(state->scene, state->spirit, obstacle,
                     elevator_user_handler, NULL, 0, NULL);
    asset_make_image_with_body(ELEVATOR_PATH, obstacle);
  }

  // make elevator button
  vector_t e_button_coord = (vector_t){E_BUTTONS[1][0], E_BUTTONS[1][1]};
  body_t *e_button = make_obstacle(E_BUTTONS[1][2], E_BUTTONS[1][3],
                                   e_button_coord, "elevator button");
  scene_add_body(state->scene, e_button);
  create_collision(state->scene, state->spirit, e_button, platform_handler,
                   NULL, 0, NULL);
  asset_make_button(ELEVATOR_BUTTON_UNPRESSED_PATH,
                    ELEVATOR_BUTTON_PRESSED_PATH, e_button);

  // make door
  vector_t door_coord = (vector_t){DOORS[1][0], DOORS[1][1]};
  body_t *door = make_obstacle(DOORS[1][2], DOORS[1][3], door_coord, "door");
  scene_add_body(state->scene, door);
  create_collision(state->scene, state->spirit, door, platform_handler, NULL, 0,
                   NULL);
  asset_make_image_with_body(DOOR_PATH, door);

  // make door button
  vector_t button_coord = (vector_t){BUTTONS[1][0], BUTTONS[1][1]};
  body_t *button =
      make_obstacle(BUTTONS[1][2], BUTTONS[1][3], button_coord, "door button");
  scene_add_body(state->scene, button);
  create_collision(state->scene, state->spirit, button, platform_handler, NULL,
                   0, NULL);
  asset_make_button(DOOR_BUTTON_UNPRESSED_PATH, DOOR_BUTTON_PRESSED_PATH,
                    button);

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

  size_t lava_len = LAVA_NUM[2];
  for (size_t i = 0; i < lava_len; i++) {
    vector_t coord = (vector_t){LAVA3[i][0], LAVA3[i][1]};
    body_t *obstacle = make_obstacle(LAVA3[i][2], LAVA3[i][3], coord, "lava");
    scene_add_body(state->scene, obstacle);
    create_collision(state->scene, state->spirit, obstacle, reset_user_handler,
                     NULL, 0, NULL);
    asset_make_anim(LAVA1_PATH, LAVA2_PATH, LAVA3_PATH, obstacle);
  }

  size_t water_len = WATER_NUM[2];
  for (size_t i = 0; i < water_len; i++) {
    vector_t coord = (vector_t){WATER3[i][0], WATER3[i][1]};
    body_t *obstacle =
        make_obstacle(WATER3[i][2], WATER3[i][3], coord, "water");
    scene_add_body(state->scene, obstacle);
    asset_make_anim(WATER1_PATH, WATER2_PATH, WATER3_PATH, obstacle);
  }

  size_t gem_len = GEM_NUM[2];
  for (size_t i = 0; i < gem_len; i++) {
    vector_t center = (vector_t){GEM3[i][0], GEM3[i][1]};
    body_t *gem = make_gem(OUTER_RADIUS, INNER_RADIUS, center);
    scene_add_body(state->scene, gem);
    create_collision(state->scene, state->spirit, gem, gem_user_handler, NULL,
                     0, NULL);
    asset_make_image_with_body(GEM_PATH, gem);
  }

  // make exit
  vector_t coord = (vector_t){EXITS[2][0], EXITS[2][1]};
  body_t *exit = make_obstacle(EXITS[2][2], EXITS[2][3], coord, "exit");
  scene_add_body(state->scene, exit);
  create_collision(state->scene, state->spirit, exit, win_handler, NULL, 0,
                   NULL);
  asset_make_image_with_body(EXIT_DOOR_PATH, exit);

  // make_clock(state);
}

void go_to_level1(state_t *state) {
  asset_reset_asset_list();
  scene_free(state->scene);
  state->scene = scene_init();
  state->current_screen = LEVEL1;
  state->elevator = false;
  // state->level_completed[0] = false;
  make_level1(state);
  return;
}

void go_to_level2(state_t *state) {
  asset_reset_asset_list();
  scene_free(state->scene);
  state->scene = scene_init();
  state->current_screen = LEVEL2;
  state->elevator = false;
  make_level2(state);
  return;
}

void go_to_level3(state_t *state) {
  asset_reset_asset_list();
  scene_free(state->scene);
  state->scene = scene_init();
  state->current_screen = LEVEL3;
  state->elevator = false;
  make_level3(state);
  return;
}

void go_to_homepage(state_t *state) {
  if (state->current_screen != HOMEPAGE) {
    asset_reset_asset_list();
    scene_free(state->scene);
    state->scene = scene_init();
  }
  state->current_screen = HOMEPAGE;
  state->pause = false;
  sdl_reset_timer();

  SDL_Rect box = (SDL_Rect){.x = MIN.x, .y = MIN.y, .w = MAX.x, .h = MAX.y};
  asset_make_image(HOMEPAGE_PATH, box);

  SDL_Rect level_gem_box[3] = {
      (SDL_Rect){.x = 148, .y = 375, .w = 50, .h = 50},
      (SDL_Rect){.x = 350, .y = 375, .w = 50, .h = 50},
      (SDL_Rect){.x = 552, .y = 375, .w = 50, .h = 50}};

  for (size_t i = 0; i < NUMBER_OF_LEVELS; i++) {
    double score = state->level_points[i];
    if (score >= GREEN_THRESHOLD && state->level_completed[i]) {
      asset_make_image(GREEN_GEM_PATH, level_gem_box[i]);
    } else if (score >= ORANGE_THRESHOLD && state->level_completed[i]) {
      asset_make_image(ORANGE_GEM_PATH, level_gem_box[i]);
    } else if (score >= RED_THRESHOLD && state->level_completed[i]) {
      asset_make_image(RED_GEM_PATH, level_gem_box[i]);
    }
  }
}

void pause(state_t *state) {
  state->pause = true;
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
  sdl_reset_timer();
  // list_remove(asset_list, list_size(asset_list) - 1);
}

void restart(state_t *state) {
  state->time = 0; // reset everything
  unpause(state);
  if (state->current_screen == LEVEL1) {
    go_to_level1(state);
  } else if (state->current_screen == LEVEL2) {
    go_to_level2(state);
  } else if (state->current_screen == LEVEL3) {
    go_to_level3(state);
  }
}

// need to fix this so that the screen transitions work well
// handle the if else cases correcly
void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  if (state->current_screen == HOMEPAGE) {
    if (type == KEY_PRESSED) {
      switch (key) {
      case KEY_1:
        go_to_level1(state);
        break;
      case KEY_2:
        if (state->level_completed[0]) {
          go_to_level2(state);
        }
        break;
      case KEY_3:
        if (state->level_completed[0] && state->level_completed[1]) {
          go_to_level3(state);
        }
        break;
      }
    }
  } else {
    list_t *asset_list = asset_get_asset_list();
    collision_type_t collision_type = state->collision_type;
    body_t *spirit = scene_get_body(state->scene, 0);
    vector_t velocity = body_get_velocity(spirit);
    asset_t *spirit_asset = list_get(asset_list, 1);
    if (type == KEY_PRESSED && !state->pause) {
      switch (key) {
      case KEY_H:
        go_to_homepage(state);
        break;
      case LEFT_ARROW:
        if (!(collision_type == RIGHT_COLLISION ||
              collision_type == UP_RIGHT_COLLISION ||
              collision_type == DOWN_RIGHT_COLLISION)) {
          body_set_velocity(spirit, (vector_t){VELOCITY_LEFT.x, velocity.y});
        }
        asset_change_texture(spirit_asset, key);
        break;
      case RIGHT_ARROW:
        if (!(collision_type == LEFT_COLLISION ||
              collision_type == UP_LEFT_COLLISION ||
              collision_type == DOWN_LEFT_COLLISION)) {
          body_set_velocity(spirit, (vector_t){VELOCITY_RIGHT.x, velocity.y});
        }
        asset_change_texture(spirit_asset, key);
        break;
      case UP_ARROW:
        sdl_play_jump_sound(JUMP_SOUND_PATH);
        if (collision_type == UP_COLLISION ||
            collision_type == UP_LEFT_COLLISION ||
            collision_type == UP_RIGHT_COLLISION) {
          body_set_velocity(spirit, (vector_t){velocity.x, VELOCITY_UP.y});
          break;
        }
        asset_change_texture(spirit_asset, key);
        break;
      case KEY_P:
        if (state->current_screen == LEVEL1 ||
            state->current_screen == LEVEL2 ||
            state->current_screen == LEVEL3) {
          pause(state);
        }
        break;
      }
    } else if (type == KEY_PRESSED && state->pause) {
      switch (key) {
      case KEY_H:
        go_to_homepage(state);
        break;
      case KEY_R:
        restart(state);
        break;
      case KEY_U:
        unpause(state);
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
      asset_change_texture(spirit_asset, UP_ARROW);
    }
  }
}

double rand_double(double low, double high) {
  return (high - low) * rand() / RAND_MAX + low;
}

void button_action(state_t *state, body_t *button) {
  list_t *asset_list = asset_get_asset_list();
  for (size_t i = 0; i < list_size(asset_list); i++) {
    asset_t *asset = list_get(asset_list, i);
    if (asset->type == ASSET_IMAGE) {
      image_asset_t *obstacle = (image_asset_t *)asset;
      body_t *body = obstacle->body;
      if ((strcmp(body_get_info(button), "door button") == 0 &&
           strcmp(body_get_info(body), "door") == 0)) {
        list_remove(asset_list, i);
        asset_destroy(obstacle);
        body_remove(body);
        break;
      } else if (strcmp(body_get_info(button), "elevator button") == 0 &&
                 strcmp(body_get_info(body), "elevator") == 0) {
        state->elevator = true;
      }
    }
  }
}

void button_press(state_t *state) {
  body_t *spirit = state->spirit;
  list_t *asset_list = asset_get_asset_list();
  for (size_t i = 0; i < list_size(asset_list); i++) {
    asset_t *asset = list_get(asset_list, i);
    if (asset->type == ASSET_BUTTON) {
      button_asset_t *button_asset = (button_asset_t *)asset;
      body_t *button = button_asset->body;
      if (find_collision(spirit, button).collided) {
        asset_change_texture_button(button_asset);
        button_action(state, button);
      }
    }
  }
}

void apply_gravity(state_t *state, double dt) {
  body_t *spirit = state->spirit;
  vector_t spirit_velocity = body_get_velocity(spirit);
  if (!(state->collision_type == UP_COLLISION ||
        state->collision_type == UP_LEFT_COLLISION ||
        state->collision_type == UP_RIGHT_COLLISION)) {
    body_set_velocity(spirit, (vector_t){spirit_velocity.x,
                                         spirit_velocity.y - (GRAVITY * dt)});
  }
}

void update_points(state_t *state) {
  size_t gem_counter = 3;
  list_t *asset_list = asset_get_asset_list();
  for (size_t i = 0; i < list_size(asset_list); i++) {
    asset_t *asset = list_get(asset_list, i);
    if (asset->type == ASSET_IMAGE) {
      image_asset_t *gem_asset = (image_asset_t *)asset;
      body_t *gem = gem_asset->body;
      if (strcmp(body_get_info(gem), "gem") == 0) {
        gem_counter--;
      }
    }
  }

  double score = pow(gem_counter, 2) * (60 / state->time);

  if (score > state->level_points[state->current_screen - 1] &&
      state->level_completed[state->current_screen - 1]) {
    state->level_points[state->current_screen - 1] = score;
  }
}

void level_complete(state_t *state) {
  body_t *spirit = state->spirit;
  list_t *asset_list = asset_get_asset_list();
  for (size_t i = 0; i < list_size(asset_list); i++) {
    asset_t *asset = list_get(asset_list, i);
    if (asset->type == ASSET_IMAGE) {
      image_asset_t *obstacle = (image_asset_t *)asset;
      body_t *body = obstacle->body;
      if ((strcmp(body_get_info(body), "exit")) == 0 &&
          find_collision(spirit, body).collided) {
        state->level_completed[state->current_screen - 1] = true;
      }
    }
  }
}

// void level_failed(state_t *state) {
//   body_t *spirit = state->spirit;
//   list_t *asset_list = asset_get_asset_list();
//   for (size_t i = 0; i < list_size(asset_list); i++) {
//     asset_t *asset = list_get(asset_list, i);
//     if (asset->type == ASSET_IMAGE) {
//       image_asset_t *lava = (image_asset_t *)asset;
//       body_t *body = lava->body;
//       if ((strcmp(body_get_info(body), "lava")) == 0 &&
//           find_collision(spirit, body).collided) {
//         state->level_failed[state->current_screen - 1] == true;
//       }
//     }
//   }
// }

collision_type_t collision(state_t *state) {
  body_t *spirit = state->spirit;
  scene_t *scene = state->scene;
  collision_type_t res = NO_COLLISION;

  for (size_t i = 1; i < scene_bodies(scene); i++) {
    body_t *platform = scene_get_body(scene, i);

    if ((strcmp(body_get_info(platform), "platform") != 0) &&
        (strcmp(body_get_info(platform), "elevator") != 0) &&
        (strcmp(body_get_info(platform), "door") != 0) &&
        (strcmp(body_get_info(platform), "door button") != 0) &&
        (strcmp(body_get_info(platform), "elevator button") != 0)) {
      continue;
    }

    if (!find_collision(spirit, platform).collided) {
      continue;
    }

    vector_t cen = body_get_centroid(spirit);
    list_t *pts = body_get_shape(platform);
    vector_t *v1 = list_get(pts, 0); // bottom left
    vector_t *v2 = list_get(pts, 1); // bottom right
    vector_t *v3 = list_get(pts, 2); // top right
    vector_t *v4 = list_get(pts, 3); // top left

    if (cen.x > v4->x - INNER_RADIUS && cen.x < v3->x + INNER_RADIUS &&
        cen.y - (INNER_RADIUS - 8) >= v4->y) {
      res += UP_COLLISION;
      continue;
    }
    if (cen.x > v1->x - INNER_RADIUS && cen.x < v2->x + INNER_RADIUS &&
        cen.y < v1->y) {
      res += DOWN_COLLISION;
      continue;
    }
    if (cen.y > v1->y - OUTER_RADIUS && cen.y < v4->y + OUTER_RADIUS &&
        cen.x < v1->x) {
      res += LEFT_COLLISION;
      continue;
    }
    if (cen.y > v2->y - OUTER_RADIUS && cen.y < v3->y + OUTER_RADIUS &&
        cen.x > v2->x) {
      res += RIGHT_COLLISION;
      continue;
    }
  }
  return res;
}

state_t *emscripten_init() {
  asset_cache_init();
  sdl_init(MIN, MAX);
  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  state->current_screen = LEVEL1;
  state->collision_type = NO_COLLISION;
  state->pause = false;
  state->elevator = false;
  state->level_points[0] = 0.0;      // for level 1
  state->level_points[1] = 0.0;      // for level 2
  state->level_points[2] = 0.0;      // for level 3
  state->level_completed[0] = false; // for level 1
  state->level_completed[1] = false; // for level 2
  state->level_completed[2] = false; // for level 3
  state->level_completed[0] = false; // for level 1
  state->level_completed[1] = false; // for level 2
  state->level_completed[2] = false; // for level 3
  state->time = 0;
  state->font = TTF_OpenFont(FONT_FILEPATH, 18);

  go_to_homepage(state);

  // moved this code to init_bgd_spirit()
  // SDL_Rect box = (SDL_Rect){.x = MIN.x, .y = MIN.y, .w = MAX.x, .h = MAX.y};
  // asset_make_image(BACKGROUND_PATH, box);

  // body_t *spirit = make_spirit(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
  // body_set_centroid(spirit, START_POS);
  // state->spirit = spirit;
  // scene_add_body(state->scene, spirit);

  // // spirit
  // asset_make_spirit(SPIRIT_FRONT_PATH, SPIRIT_LEFT_PATH, SPIRIT_RIGHT_PATH,
  //                   spirit);

  // make level
  // make_level1(state);
  // make_level2(state);
  // make_level3(state);

  // make_clock(state);

  sdl_on_key((key_handler_t)on_key);

  return state;
}

bool emscripten_main(state_t *state) {
  // check what screen it is on
  sdl_clear();
  sdl_render_scene(state->scene);
  sdl_play_music(BACKGROUND_MUSIC_PATH);
  list_t *body_assets = asset_get_asset_list();
  size_t len = list_size(body_assets);

  // level_failed(state);

  // bool game_over = false;
  // if (state->level_completed[state->current_screen - 1] ||
  //     state->level_failed[state->current_screen - 1]) {
  //   game_over = true;
  // }

  // if ((state->current_screen == LEVEL1 && !(state->level_completed[0])) ||
  // (state->current_screen == LEVEL2 && !(state->level_completed[1])) ||
  // (state->current_screen == LEVEL3 && !(state->level_completed[2]))) {
  //   game_over = true;
  // }

  // for loop
  for (size_t i = 0; i < len; i++) {
    asset_t *asset = list_get(body_assets, i);
    asset_animate(asset, state->time);
    asset_render(list_get(body_assets, i));
  }

  if (state->current_screen != HOMEPAGE) {

    if (!(state->pause)) {

      char text[10000];
      sprintf(text, "Clock:%.0f", floor(state->time));
      vector_t text_dim = get_dimensions_for_text(text);
      SDL_Rect rect = (SDL_Rect){.x = CLOCK_POS.x - (text_dim.x / 2),
                                 .y = CLOCK_POS.y,
                                 .w = text_dim.x,
                                 .h = text_dim.y};

      sdl_render_text(text, state->font, CLOCK_COL, &rect);

      state->collision_type = collision(state);
      double dt = time_since_last_tick();

      // apply gravity
      if (dt < 0.2) {
        apply_gravity(state, dt);
      }

      // check for pressed buttons
      button_press(state);

      // move elevator
      if (state->elevator) {
        move_elevator(state);
      }

      // update points
      update_points(state);

      // check for completed level
      level_complete(state);

      // clocks
      // asset_t *clock = list_get(body_assets, len - 1);
      // asset_destroy(clock);
      // list_remove(body_assets, len - 1);
      // update_clock(state);
      scene_tick(state->scene, dt);
      // make_clock(state);
      state->time += dt;
    }

    // asset_destroy(clock); // only destroy if the clock is there

    // sdl_render_scene(state->scene);
  }

  // body_t *elevator = scene_get_body(state->scene, 1);
  // move_elevator(elevator);

  // size_t time = (size_t)state->time;
  // if (time % 10 != 0) {
  //   state->music_played = false;
  // }

  // if ((time % 10 == 0) && (!(state->music_played))) {
  //   sdl_play_music(BACKGROUND_MUSIC_PATH);
  //   state->music_played = true;
  // }

  sdl_show();
  return false;
}

void emscripten_free(state_t *state) {
  sdl_quit(); // added by Natalie
  list_free(asset_get_asset_list());
  scene_free(state->scene); // does this destroy all the bodies?
  asset_cache_destroy();
  free(state);
}
