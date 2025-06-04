#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "asset_cache.h"
#include "color.h"
#include "sdl_wrapper.h"

static list_t *ASSET_LIST = NULL;
const size_t INIT_CAPACITY = 5;

typedef struct asset {
  asset_type_t type;
  SDL_Rect bounding_box;
} asset_t;

typedef struct text_asset {
  asset_t base;
  TTF_Font *font;
  const char *text;
  color_t color;
} text_asset_t;

typedef struct image_asset {
  asset_t base;
  SDL_Texture *texture;
  body_t *body;
} image_asset_t;

typedef struct spirit_asset {
  asset_t base;
  SDL_Texture *curr_texture;
  SDL_Texture *front_texture;
  SDL_Texture *right_texture;
  SDL_Texture *left_texture;
  body_t *body;
} spirit_asset_t;

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
  asset_t *new =
      malloc(ty == ASSET_IMAGE ? sizeof(image_asset_t) : sizeof(text_asset_t));
  // : sizeof(spirit_asset_t)
  assert(new);
  new->type = ty;
  new->bounding_box = bounding_box;
  return new;
}

void asset_make_image_with_body(const char *filepath, body_t *body) {
  SDL_Rect bounding_box = sdl_get_bounding_box(body);
  SDL_Texture *image_texture =
      (SDL_Texture *)asset_cache_obj_get_or_create(ASSET_IMAGE, filepath);
  asset_t *asset = asset_init(ASSET_IMAGE, bounding_box);
  image_asset_t *image_asset = (image_asset_t *)asset;
  image_asset->texture = image_texture;
  image_asset->body = body;
  list_add(ASSET_LIST, (asset_t *)image_asset);
}

void asset_make_image(const char *filepath, SDL_Rect bounding_box) {
  SDL_Texture *image_texture =
      (SDL_Texture *)asset_cache_obj_get_or_create(ASSET_IMAGE, filepath);
  asset_t *asset = asset_init(ASSET_IMAGE, bounding_box);
  image_asset_t *image_asset = (image_asset_t *)asset;
  image_asset->texture = image_texture;
  image_asset->body = NULL;
  list_add(ASSET_LIST, (asset_t *)image_asset);
}

void asset_make_text(const char *filepath, SDL_Rect bounding_box,
                     const char *text, color_t color) {
  TTF_Font *text_font =
      (TTF_Font *)asset_cache_obj_get_or_create(ASSET_TEXT, filepath);
  asset_t *asset = asset_init(ASSET_TEXT, bounding_box);
  text_asset_t *text_asset = (text_asset_t *)asset;
  text_asset->font = text_font;
  text_asset->text = text;
  text_asset->color = color;
  list_add(ASSET_LIST, (asset_t *)text_asset);
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
  spirit_asset->body = NULL;
  list_add(ASSET_LIST, (asset_t *)spirit_asset);
}

void *asset_change_texture(asset_t *asset, size_t idx) {
  assert(asset->type == ASSET_SPIRIT);
  spirit_asset_t *spirit_asset = (spirit_asset_t *)asset;
  if (idx == 0) {
    spirit_asset->curr_texture = spirit_asset->front_texture;
  } else if (idx == 1) {
    spirit_asset->curr_texture = spirit_asset->right_texture;
  } else if (idx == 2) {
    spirit_asset->curr_texture = spirit_asset->left_texture;
  }
}

void asset_reset_asset_list() {
  if (ASSET_LIST != NULL) {
    list_free(ASSET_LIST);
  }
  ASSET_LIST = list_init(INIT_CAPACITY, (free_func_t)asset_destroy);
}

list_t *asset_get_asset_list() { return ASSET_LIST; }

void asset_remove_body(body_t *body) {
  size_t size = list_size(ASSET_LIST);
  for (ssize_t i = size - 1; i >= 0; i--) {
    asset_t *asset = list_get(ASSET_LIST, i);
    if (asset->type == ASSET_IMAGE &&
        (((image_asset_t *)asset)->body == body)) {
      asset = list_remove(ASSET_LIST, i);
      asset_destroy(asset);
    }
  }
}

void asset_render(asset_t *asset) {
  if (asset->type == ASSET_IMAGE) {
    if (((image_asset_t *)asset)->body == NULL) {
      sdl_render_image(((image_asset_t *)asset)->texture, asset->bounding_box);
    } else {
      sdl_render_image(((image_asset_t *)asset)->texture,
                       sdl_get_bounding_box(((image_asset_t *)asset)->body));
    }
<<<<<<< HEAD

    sdl_render_image(image->texture, &box);
    break;
  case ASSET_TEXT:
    text_asset_t *text_asset = (text_asset_t *)asset;
    sdl_render_text(text_asset->text, text_asset->font, text_asset->color,
                    &box);
    break;
  case ASSET_SPIRIT:
    spirit_asset_t *spirit_asset = (spirit_asset_t *)asset;
    sdl_render_image(spirit_asset->curr_texture, &box);
=======
  }
  if (asset->type == ASSET_TEXT) {
    sdl_render_text(((text_asset_t *)asset)->font,
                    ((char *)((text_asset_t *)asset)->text),
                    &(asset->bounding_box), ((text_asset_t *)asset)->color);
>>>>>>> refs/remotes/origin/master
  }
}

void asset_destroy(asset_t *asset) { free(asset); }