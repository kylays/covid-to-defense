#ifndef __SCENE_H__
#define __SCENE_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "image.h"
#include "body.h"
#include "list.h"
#include "text.h"
// #include "global_body_info.h"

/**
 * A collection of bodies and force creators.
 * The scene automatically resizes to store
 * arbitrarily many bodies and force creators.
 */
typedef struct scene scene_t;

/**
 * A function which adds some forces or impulses to bodies,
 * e.g. from collisions, gravity, or spring forces.
 * Takes in an auxiliary value that can store parameters or state.
 */
typedef void (*force_creator_t)(void *aux);

/**
 * Allocates memory for an empty scene.
 * Makes a reasonable guess of the number of bodies to allocate space for.
 * Asserts that the required memory is successfully allocated.
 *
 * @return the new scene
 */
scene_t *scene_init(void);

/**
 * Releases memory allocated for a given scene
 * and all the bodies and force creators it contains.
 *
 * @param scene a pointer to a scene returned from scene_init()
 */
void scene_free(scene_t *scene);

/**
 * Gets the number of bodies in a given scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @return the number of bodies added with scene_add_body()
 */
size_t scene_bodies(scene_t *scene);

/**
 * Gets the number of bodies in a given scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @return the bodies added with scene_add_body()
 */
list_t *scene_get_bodies(scene_t *scene);

/**
 * Gets the body at a given index in a scene.
 * Asserts that the index is valid.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param index the index of the body in the scene (starting at 0)
 * @return a pointer to the body at the given index
 */
body_t *scene_get_body(scene_t *scene, size_t index);

/**
 * Adds a body to a scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param body a pointer to the body to add to the scene
 */
void scene_add_body(scene_t *scene, body_t *body);

/**
 * @deprecated Use body_remove() instead
 *
 * Removes and frees the body at a given index from a scene.
 * Asserts that the index is valid.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param index the index of the body in the scene (starting at 0)
 */
void scene_remove_body(scene_t *scene, size_t index);

/**
 * @deprecated Use scene_add_bodies_force_creator() instead
 * so the scene knows which bodies the force creator depends on
 */
void scene_add_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    free_func_t freer);

/**
 * Adds a force creator to a scene,
 * to be invoked every time scene_tick() is called.
 * The auxiliary value is passed to the force creator each time it is called.
 * The force creator is registered with a list of bodies it applies to,
 * so it can be removed when any one of the bodies is removed.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param forcer a force creator function
 * @param aux an auxiliary value to pass to forcer when it is called
 * @param bodies the list of bodies affected by the force creator.
 *   The force creator will be removed if any of these bodies are removed.
 *   This list does not own the bodies, so its freer should be NULL.
 * @param freer if non-NULL, a function to call in order to free aux
 */
void scene_add_bodies_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    list_t *bodies,
    free_func_t freer);

/**
 * Executes a tick of a given scene over a small time interval.
 * This requires executing all the force creators
 * and then ticking each body (see body_tick()).
 * If any bodies are marked for removal, they should be removed from the scene
 * and freed, along with any force creators acting on them.
 *
 * @param scene a pointer to the scene returned from scene_init()
 * @param dt the time elapsed since the last tick, in seconds
 */
void scene_tick(scene_t *scene, double dt);

/**
 * @brief returns the text at index i in scene->texts
 * 
 * @param scene 
 * @param i 
 * @return text_t* 
 */
text_t *scene_get_text(scene_t *scene, size_t i);

/**
 * @brief returns the number of texts in scene->texts
 * 
 * @param scene 
 * @return size_t 
 */
size_t scene_texts(scene_t *scene);

/**
 * @brief adds a text to the list of texts in the scene
 * 
 * @param scene 
 * @param text 
 */
void scene_add_text(scene_t *scene, text_t *text);

/**
 * @brief removes a text from the scene by removing from the list
 * 
 * @param scene 
 * @param i 
 */
void scene_remove_text(scene_t *scene, size_t i);

/**
 * @brief returns a pointer to the image at index i in list scene->images
 * 
 * @param scene 
 * @param i 
 * @return image_t* 
 */
image_t *scene_get_image(scene_t *scene, size_t i);

/**
 * @brief returns the number of images in the scene
 * 
 * @param scene 
 * @return size_t 
 */
size_t scene_images(scene_t *scene);

/**
 * @brief adds an image to the scene by adding to scene->images
 * 
 * @param scene 
 * @param image 
 */
/**
 * @brief Adds n image to the scene
 * 
 * @param scene 
 * @param image 
 */
void scene_add_image(scene_t *scene, image_t *image);

/**
 * @brief Removes the image from the scene
 * 
 * @param scene 
 * @param i 
 */
void scene_remove_image(scene_t *scene, size_t i);

/**
 * @brief Frees SDL stuff
 * 
 */
void sdl_cleanup();

/**
 * @brief removes all bodies, images, and text
 * 
 * @param scene 
 */
void scene_clear(scene_t *scene);

#endif // #ifndef __SCENE_H__
