#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset_cache.h"
#include "list.h"
#include "sdl_wrapper.h"

static list_t *ASSET_CACHE;

const size_t FONT_SIZE = 18;
const size_t INITIAL_CAPACITY = 5;

// copied from olivia's project 5

typedef struct {
  asset_type_t type;
  const char *filepath;
  void *obj;
} entry_t;

static void asset_cache_free_entry(entry_t *entry) {
  switch (entry->type) {
  case ASSET_IMAGE:
    SDL_DestroyTexture(entry->obj);
  case ASSET_TEXT:
    TTF_CloseFont(entry->obj);
  }
  free(entry);
}

void asset_cache_init() {
  ASSET_CACHE =
      list_init(INITIAL_CAPACITY, (free_func_t)asset_cache_free_entry);
}

void asset_cache_destroy() { list_free(ASSET_CACHE); }

void *asset_exists(const char *filepath) {
  size_t len = list_size(ASSET_CACHE);
  for (size_t i = 0; i < len; i++) {
    entry_t *entry = (entry_t *)list_get(ASSET_CACHE, i);
    if (strcmp(entry->filepath, filepath) == 0) {
      return entry->obj;
    }
  }
  return NULL;
}

void *asset_change_texture(const char *filepath, size_t idx){
  entry_t *entry = (entry_t *) list_get(ASSET_CACHE, idx);
  SDL_Texture *old_texture = entry->obj;
  SDL_DestroyTexture(old_texture);
  entry->obj = sdl_get_image_texture(filepath);
  return entry->obj;
}

void *asset_cache_obj_get_or_create(asset_type_t ty, const char *filepath) {
  // Hints: Create a helper function to check if an entry already corresponds to
  // `filepath`. If it does, you're good to go.
  // Otherwise, you will have to initialize the asset that corresponds to `ty`.
  // What file helps us with initializing assets?
  // You should NOT be returning a pointer to an `entry_t`!
  void *obj = asset_exists(filepath);
  if (obj == NULL) {
    entry_t *entry = malloc(sizeof(entry_t));
    entry->type = ty;
    entry->filepath = filepath;
    switch (ty) {
    case ASSET_IMAGE:
      entry->obj = sdl_get_image_texture(filepath);
      break;
    case ASSET_TEXT: {
      entry->obj = TTF_OpenFont(filepath, FONT_SIZE);
      break;
    }
    }

    list_add(ASSET_CACHE, entry);
    return entry->obj;
  }

  return obj;
}
