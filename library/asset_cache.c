#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset_cache.h"
#include "list.h"
#include "sdl_wrapper.h"

static list_t *ASSET_CACHE;

const size_t FONT_SIZE = 18;
const size_t INITIAL_CAPACITY = 5;

typedef struct {
  asset_type_t type;
  const char *filepath;
  void *obj;
} entry_t;

static void asset_cache_free_entry(entry_t *entry) {
  if (entry->type == ASSET_IMAGE) {
    SDL_DestroyTexture(entry->obj);
  }
  if (entry->type == ASSET_TEXT) {
    TTF_CloseFont(entry->obj);
  }
  free(entry);
}

void asset_cache_init() {
  ASSET_CACHE =
      list_init(INITIAL_CAPACITY, (free_func_t)asset_cache_free_entry);
}

void asset_cache_destroy() { list_free(ASSET_CACHE); }

// helper function
void *entry_corresponds_to_filepath(asset_type_t ty, const char *filepath) {
  size_t cache_size = list_size(ASSET_CACHE);
  for (size_t i = 0; i < cache_size; i++) {
    entry_t *entry = list_get(ASSET_CACHE, i);
    if (strcmp(entry->filepath, filepath) == 0 && entry->type == ty) {
      return entry->obj;
    }
  }
  return NULL;
}

void *asset_cache_obj_get_or_create(asset_type_t ty, const char *filepath) {
  void *get_entry = entry_corresponds_to_filepath(ty, filepath);
  if (get_entry != NULL) {
    return get_entry;
  }
  entry_t *new_entry = malloc(sizeof(entry_t));
  new_entry->type = ty;
  new_entry->filepath = filepath;
  if (new_entry->type == ASSET_IMAGE) {
    new_entry->obj = sdl_get_image_texture(filepath);
  }
  if (new_entry->type == ASSET_TEXT) {
    new_entry->obj = TTF_OpenFont(filepath, FONT_SIZE);
  }
  list_add(ASSET_CACHE, new_entry);
  return new_entry->obj;
}
