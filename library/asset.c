#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "asset_cache.h"
#include "color.h"
#include "sdl_wrapper.h"

static list_t *ASSET_LIST = NULL;
const size_t INIT_CAPACITY = 10;

/**
 * Allocates memory for an asset with the given parameters.
 *
 * @param ty the type of the asset
 * @param bounding_box the bounding box containing the location and dimensions
 * of the asset when it is rendered
 * @return a pointer to the newly allocated asset
 */
static asset_t *asset_init(asset_type_t ty, SDL_Rect bounding_box) {
  // This is a fancy way of malloc'ing space for an `image_asset_t` if `ty` is
  // ASSET_IMAGE, and `text_asset_t` otherwise.
  if (ASSET_LIST == NULL) {
    ASSET_LIST = list_init(INIT_CAPACITY, (free_func_t)asset_destroy);
  }
  asset_t *new = NULL;
  switch (ty) {
  case ASSET_IMAGE:
    new = malloc(sizeof(image_asset_t));
    break;
  case ASSET_TEXT:
    new = malloc(sizeof(text_asset_t));
    break;
  case ASSET_SPIRIT:
    new = malloc(sizeof(spirit_asset_t));
    break;
  case ASSET_BUTTON:
    new = malloc(sizeof(button_asset_t));
    break;
  case ASSET_ANIM:
    new = malloc(sizeof(anim_asset_t));
    break;
  }
  assert(new);
  new->type = ty;
  new->bounding_box = bounding_box;
  return new;
}

void asset_make_image_with_body(const char *filepath, body_t *body) {
  SDL_Rect bounding_box = (SDL_Rect){.x = 0, .y = 0, .w = 0, .h = 0};
  asset_t *asset = asset_init(ASSET_IMAGE, bounding_box);
  image_asset_t *image_asset = (image_asset_t *)asset;
  image_asset->texture = asset_cache_obj_get_or_create(ASSET_IMAGE, filepath);
  image_asset->body = body;
  list_add(ASSET_LIST, (asset_t *)image_asset);
}

void asset_make_image(const char *filepath, SDL_Rect bounding_box) {
  asset_t *asset = asset_init(ASSET_IMAGE, bounding_box);
  image_asset_t *image_asset = (image_asset_t *)asset;
  image_asset->texture = asset_cache_obj_get_or_create(ASSET_IMAGE, filepath);
  image_asset->body = NULL;
  list_add(ASSET_LIST, (asset_t *)image_asset);
}

void asset_make_text_with_body(const char *filepath, const char *text,
                               color_t color, body_t *body) {
  SDL_Rect bounding_box = (SDL_Rect){.x = 0, .y = 0, .w = 0, .h = 0};
  asset_t *asset = asset_init(ASSET_TEXT, bounding_box);
  text_asset_t *text_asset = (text_asset_t *)asset;
  text_asset->font = asset_cache_obj_get_or_create(ASSET_TEXT, filepath);
  text_asset->text = text;
  text_asset->color = color;
  text_asset->body = body;
  list_add(ASSET_LIST, (asset_t *)text_asset);
}

void asset_make_text_body(const char *filepath, const char *text, color_t color,
                          body_t *body) {
  SDL_Rect bounding_box = (SDL_Rect){.x = 0, .y = 0, .w = 0, .h = 0};
  asset_t *asset = asset_init(ASSET_TEXT, bounding_box);
  text_asset_t *text_asset = (text_asset_t *)asset;
  text_asset->font = asset_cache_obj_get_or_create(ASSET_TEXT, filepath);
  text_asset->text = text;
  text_asset->body = body;
  list_add(ASSET_LIST, (asset_t *)text_asset);
}

void asset_make_text(const char *filepath, SDL_Rect bounding_box,
                     const char *text, color_t color) {
  asset_t *asset = asset_init(ASSET_TEXT, bounding_box);
  text_asset_t *text_asset = (text_asset_t *)asset;
  text_asset->font = asset_cache_obj_get_or_create(ASSET_TEXT, filepath);
  text_asset->text = text;
  text_asset->color = color;
  text_asset->body = NULL;
  list_add(ASSET_LIST, (asset_t *)text_asset);
}

void asset_update_text(asset_t *asset, const char *text) {
  if (asset->type == ASSET_TEXT) {
    text_asset_t *text_asset = (text_asset_t *)asset;
    text_asset->text = text;
  }
}

// new asset for the spirit
void asset_make_spirit(const char *front_filepath, const char *left_filepath,
                       const char *right_filepath, body_t *body) {
  SDL_Rect bounding_box = (SDL_Rect){.x = 0, .y = 0, .w = 0, .h = 0};
  asset_t *asset = asset_init(ASSET_SPIRIT, bounding_box);
  spirit_asset_t *spirit_asset = (spirit_asset_t *)asset;
  spirit_asset->front_texture =
      asset_cache_obj_get_or_create(ASSET_IMAGE, front_filepath);
  spirit_asset->right_texture =
      asset_cache_obj_get_or_create(ASSET_IMAGE, right_filepath);
  spirit_asset->left_texture =
      asset_cache_obj_get_or_create(ASSET_IMAGE, left_filepath);
  spirit_asset->curr_texture = spirit_asset->front_texture;
  spirit_asset->body = body;
  list_add(ASSET_LIST, (asset_t *)spirit_asset);
}

void asset_make_anim(const char *frame1_filepath, const char *frame2_filepath,
                     const char *frame3_filepath, body_t *body) {
  SDL_Rect bounding_box = (SDL_Rect){.x = 0, .y = 0, .w = 0, .h = 0};
  asset_t *asset = asset_init(ASSET_ANIM, bounding_box);
  anim_asset_t *anim_asset = (anim_asset_t *)asset;
  anim_asset->frame1_texture =
      asset_cache_obj_get_or_create(ASSET_IMAGE, frame1_filepath);
  anim_asset->frame2_texture =
      asset_cache_obj_get_or_create(ASSET_IMAGE, frame2_filepath);
  anim_asset->frame3_texture =
      asset_cache_obj_get_or_create(ASSET_IMAGE, frame3_filepath);
  anim_asset->curr_texture = anim_asset->frame1_texture;
  anim_asset->body = body;
  list_add(ASSET_LIST, (asset_t *)anim_asset);
}

void asset_change_texture(asset_t *asset, char key) {
  // pass in the key instead
  assert(asset->type == ASSET_SPIRIT);
  spirit_asset_t *spirit_asset = (spirit_asset_t *)asset;
  switch (key) {
  case LEFT_ARROW:
    spirit_asset->curr_texture = spirit_asset->left_texture;
    break;
  case RIGHT_ARROW:
    spirit_asset->curr_texture = spirit_asset->right_texture;
    break;
  case UP_ARROW:
    spirit_asset->curr_texture = spirit_asset->front_texture;
    break;
  }
}

void asset_animate(asset_t *asset, double time) {
  // pass in the key instead
  // assert(asset->type == ASSET_ANIM);
  if (asset->type == ASSET_ANIM) {
    anim_asset_t *anim_asset = (anim_asset_t *)asset;
    int val = ((int)floor(time) / 1) % 3;
    if (val == 0) {
      anim_asset->curr_texture = anim_asset->frame1_texture;
    } else if (val == 1) {
      anim_asset->curr_texture = anim_asset->frame2_texture;
    } else {
      anim_asset->curr_texture = anim_asset->frame3_texture;
    }
  }
}

void asset_make_button(const char *unpressed_filepath,
                       const char *pressed_filepath, body_t *body) {
  SDL_Rect bounding_box = {.x = 0, .y = 0, .w = 0, .h = 0};
  asset_t *asset = asset_init(ASSET_BUTTON, bounding_box);
  button_asset_t *button_asset = (button_asset_t *)asset;
  button_asset->unpressed_texture =
      asset_cache_obj_get_or_create(ASSET_IMAGE, unpressed_filepath);
  button_asset->pressed_texture =
      asset_cache_obj_get_or_create(ASSET_IMAGE, pressed_filepath);
  button_asset->curr_texture = button_asset->unpressed_texture;
  button_asset->body = body;
  list_add(ASSET_LIST, (asset_t *)button_asset);
}

void asset_change_texture_button(asset_t *asset) {
  assert(asset->type == ASSET_BUTTON);
  button_asset_t *button_asset = (button_asset_t *)asset;
  button_asset->curr_texture = button_asset->pressed_texture;
}

void asset_reset_asset_list() {
  if (ASSET_LIST != NULL) {
    list_free(ASSET_LIST);
  }
  ASSET_LIST = list_init(INIT_CAPACITY, (free_func_t)asset_destroy);
}

list_t *asset_get_asset_list() { return ASSET_LIST; }

void asset_remove_body(body_t *body) {
  size_t len = list_size(ASSET_LIST);
  for (size_t i = 0; i < len; i++) {
    asset_t *asset = list_get(ASSET_LIST, i);
    if (asset->type == ASSET_IMAGE) {
      image_asset_t *image_asset = (image_asset_t *)asset;
      if (image_asset->body == body) {
        list_remove(ASSET_LIST, i);
        asset_destroy(asset);
        i--;
        len--;
      }
    }
  }
}

void asset_render(asset_t *asset) {
  SDL_Rect box = asset->bounding_box;
  switch (asset->type) {
  case ASSET_IMAGE:
    image_asset_t *image = (image_asset_t *)asset;
    if (image->body != NULL) {
      box = sdl_get_body_bounding_box(image->body);
    }
    sdl_render_image(image->texture, &box);
    break;
  case ASSET_TEXT:
    text_asset_t *text_asset = (text_asset_t *)asset;
    if (text_asset->body != NULL) {
      box = sdl_get_body_bounding_box(image->body);
    }
    sdl_render_text(text_asset->text, text_asset->font, text_asset->color,
                    &box);
    break;
  case ASSET_SPIRIT:
    spirit_asset_t *spirit_asset = (spirit_asset_t *)asset;
    if (spirit_asset->body != NULL) {
      box = sdl_get_body_bounding_box(spirit_asset->body);
    }
    sdl_render_image(spirit_asset->curr_texture, &box);
    break;
  case ASSET_BUTTON:
    button_asset_t *button_asset = (button_asset_t *)asset;
    if (button_asset->body != NULL) {
      box = sdl_get_body_bounding_box(button_asset->body);
    }
    sdl_render_image(button_asset->curr_texture, &box);
    break;
  case ASSET_ANIM:
    anim_asset_t *anim_asset = (anim_asset_t *)asset;
    if (anim_asset->body != NULL) {
      box = sdl_get_body_bounding_box(anim_asset->body);
    }
    sdl_render_image(anim_asset->curr_texture, &box);
    break;
  }
}

void asset_destroy(asset_t *asset) { free(asset); }