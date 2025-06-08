#ifndef __ASSET_H__
#define __ASSET_H__

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <color.h>
#include <sdl_wrapper.h>
#include <stddef.h>

#include "body.h"

typedef enum {
  ASSET_IMAGE,
  ASSET_TEXT,
  ASSET_SPIRIT,
  ASSET_ANIM,
  ASSET_BUTTON,
} asset_type_t;

typedef struct asset {
  asset_type_t type;
  SDL_Rect bounding_box;
} asset_t;

typedef struct text_asset {
  asset_t base;
  TTF_Font *font;
  const char *text;
  color_t color;
  body_t *body;
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

typedef struct anim_asset {
  asset_t base;
  SDL_Texture *curr_texture;
  SDL_Texture *frame1_texture;
  SDL_Texture *frame2_texture;
  SDL_Texture *frame3_texture;
  body_t *body;
} anim_asset_t;

typedef struct button_asset {
  asset_t base;
  SDL_Texture *curr_texture;
  SDL_Texture *unpressed_texture;
  SDL_Texture *pressed_texture;
  body_t *body;
} button_asset_t;

/**
 * Allocates memory for an image asset with the given parameters and adds it
 * to the internal asset list.
 *
 * @param filepath the filepath to the image file
 * @param bounding_box the bounding box containing the location and dimensions
 * of the text when it is rendered
 */
void asset_make_image(const char *filepath, SDL_Rect bounding_box);

/**
 * Allocates memory for an image asset with an attached body and adds it
 * to the internal asset list. When the asset is rendered, the image will be
 * rendered on top of the body.
 *
 * @param filepath the filepath to the image file
 * @param body the body to render the image on top of
 */
void asset_make_image_with_body(const char *filepath, body_t *body);

/**
 * Allocates memory for a text asset with the given parameters and adds it
 * to the internal asset list.
 *
 * @param filepath the filepath to the .ttf file
 * @param bounding_box the bounding box containing the location and dimensions
 * of the text when it is rendered
 * @param text the text to render
 * @param color the color of the text
 */
void asset_make_text(const char *filepath, SDL_Rect bounding_box,
                     const char *text, color_t color);

/**
 * Allocates memory for a text asset with the given parameters and adds it
 * to the internal asset list.
 *
 * @param filepath the filepath to the .ttf file
 * @param bounding_box the bounding box containing the location and dimensions
 * of the text when it is rendered
 * @param text the text to render
 * @param color the color of the text
 */
void asset_make_text_body(const char *filepath, const char *text, color_t color,
                          body_t *body);

void asset_update_text(asset_t *asset, const char *text);
/**
 * Allocates memory for a spirit asset with the given parameters and adds it
 * to the internal asset list.
 *
 * @param front_filepath filepath to .png for front view of spirit
 * @param left_filepath filepath to .png for left view of spirit
 * @param right_filepath filepath to lpng for right view of spirit
 * @param body the body to render the image on top of
 */
void asset_make_spirit(const char *front_filepath, const char *left_filepath,
                       const char *right_filepath, body_t *body);

/**
 * Allocates memory for an animation asset with the given parameters and
 * adds it to the internal asset list.
 *
 * @param frame1_filepath filepath to .png for front view of spirit
 * @param frame2_filepath filepath to .png for left view of spirit
 * @param frame3_filepath filepath to lpng for right view of spirit
 * @param body the body to render the image on top of
 */
void asset_make_anim(const char *frame1_filepath, const char *frame2_filepath,
                     const char *frame3_filepath, body_t *body);

/**
 * Changes the texture of the spirit asset based on the key pressed by user.
 * @param asset spirit asset
 * @param key pressed key
 */
void asset_change_texture(asset_t *asset, char key);

/**
 * Changes the texture of the animated assets for water and lava
 * based on the time.
 * @param asset animated asset
 * @param time the current time of the game
 */
void asset_animate(asset_t *asset, double time);

/**
 * Resets the internal asset list by freeing all assets and creating a new empty
 * list. This is useful when transitioning between scenes or levels.
 */
void asset_reset_asset_list();

/**
 * Allocates memory for an button asset with the given parameters and
 * adds it to the internal asset list.
 *
 * @param unpressed_filepath filepath to .png for unpressed button
 * @param pressed_filepath filepath to .png for pressed button
 * @param body the body to render the image on top of
 */
void asset_make_button(const char *unpressed_filepath,
                       const char *pressed_filepath, body_t *body);

/**
 * Changes the texture of the button asset to the pressed button.
 * This function is called when the spirit collides with the button.
 * @param asset button asset
 */
void asset_change_texture_button(asset_t *asset);

/**
 * Returns the internal list of all assets that have been created.
 *
 * @return a pointer to the list containing all assets
 */
list_t *asset_get_asset_list();

/**
 * Removes and destroys all image assets associated with the given body.
 * This is typically called when a body is destroyed to clean up its visual
 * representation.
 *
 * @param body the body whose associated assets should be removed
 */
void asset_remove_body(body_t *body);

/**
 * Renders the asset to the screen.
 * @param asset the asset to render
 */
void asset_render(asset_t *asset);

/**
 * Frees the memory allocated for the asset.
 * @param asset the asset to free
 */
void asset_destroy(asset_t *asset);

#endif // #ifndef __ASSET_H__
