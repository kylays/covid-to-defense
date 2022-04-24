#include "scene.h"

const size_t DEFAULT_NUM_BODIES = 10;
const size_t DEFAULT_NUM_FORCE_CREATORS = 5;

typedef struct scene
{
    list_t *bodies;
    list_t *force_packages;
    list_t *texts;
    list_t *images;
} scene_t;

typedef struct force_package
{
    force_creator_t forcer;
    void *aux;
    list_t *bodies;
    free_func_t freer;
} force_package_t;

void force_package_free(force_package_t *force_package)
{
    if (force_package->freer != NULL)
    {
        force_package->freer(force_package->aux);
    }
    if (force_package->bodies != NULL)
    {
        list_free(force_package->bodies);
    }
    free(force_package);
}

scene_t *scene_init(void)
{
    scene_t *s = malloc(sizeof(scene_t));
    assert(s != NULL);
    s->bodies = list_init(DEFAULT_NUM_BODIES, (free_func_t)body_free);
    s->force_packages = list_init(DEFAULT_NUM_FORCE_CREATORS, (free_func_t)force_package_free);
    s->texts = list_init(DEFAULT_NUM_BODIES, (free_func_t)text_free);
    s->images = list_init(DEFAULT_NUM_BODIES, (free_func_t)image_free);
    return s;
}

void scene_free(scene_t *scene)
{
    list_free(scene->bodies);
    list_free(scene->force_packages);
    list_free(scene->texts);
    list_free(scene->images);
    free(scene);
}

size_t scene_bodies(scene_t *scene)
{
    assert(scene->bodies != NULL);
    return list_size(scene->bodies);
}

list_t *scene_get_bodies(scene_t *scene)
{
    return scene->bodies;
}

body_t *scene_get_body(scene_t *scene, size_t index)
{
    assert(index < scene_bodies(scene));
    return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body)
{
    list_add(scene->bodies, body);
    image_t *image = body_get_image(body);
    text_t *label = body_get_label(body);
    if (image)
    {
        scene_add_image(scene, image);
    }
    if (label)
    {
        scene_add_text(scene, label);
    }
}

// DEPRECATED
void scene_remove_body(scene_t *scene, size_t index)
{
    body_remove(scene_get_body(scene, index));
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies, free_func_t freer)
{
    force_package_t *force = malloc(sizeof(force_package_t));
    assert(force != NULL);
    list_t *force_bodies;
    if (bodies != NULL)
    {
        force_bodies = list_init(list_size(bodies), NULL);
        for (size_t i = 0; i < list_size(bodies); i++)
        {
            list_add(force_bodies, list_get(bodies, i));
        }
    }
    else
    {
        force_bodies = bodies;
    }
    *force = (force_package_t){.forcer = forcer, .aux = aux, .freer = freer, .bodies = force_bodies};
    list_add(scene->force_packages, force);
}

// DEPRECATED
void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer)
{
    scene_add_bodies_force_creator(scene, forcer, aux, NULL, freer);
}

void scene_check_remove_flags(scene_t *scene)
{
    size_t fp_index = 0;
    while (fp_index < (list_size(scene->force_packages)))
    {
        force_package_t *fp = list_get(scene->force_packages, fp_index);
        list_t *bodies = fp->bodies;

        if (bodies != NULL)
        {
            for (size_t j = 0; j < list_size(bodies); j++)
            {
                if (body_is_removed(list_get(bodies, j)))
                {
                    force_package_free(list_remove(scene->force_packages, fp_index));
                    fp_index--;
                    break;
                }
            }
        }
        fp_index++;
    }

    size_t body_index = 0;
    while (body_index < scene_bodies(scene))
    {
        body_t *body = scene_get_body(scene, body_index);
        if (body_is_removed(body))
        {
            body_free(list_remove(scene->bodies, body_index));
        }
        else
        {
            body_index++;
        }
    }

    int text_index = 0;
    while (text_index < scene_texts(scene))
    {
        text_t *text = scene_get_text(scene, text_index);
        if (text->removed)
        {
            text_free(list_remove(scene->texts, text_index));
        }
        else
        {
            text_index++;
        }
    }

    int image_index = 0;
    while (image_index < scene_images(scene))
    {
        image_t *image = scene_get_image(scene, image_index);
        if (image->removed)
        {
            image_free(list_remove(scene->images, image_index));
        }
        else
        {
            image_index++;
        }
    }
}

void scene_tick(scene_t *scene, double dt)
{
    for (size_t i = 0; i < list_size(scene->force_packages); i++)
    {
        force_package_t *force_package = list_get(scene->force_packages, i);
        force_package->forcer(force_package->aux);
    }

    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        body_tick(scene_get_body(scene, i), dt);
    }

    scene_check_remove_flags(scene);
}

size_t scene_texts(scene_t *scene)
{
    return list_size(scene->texts);
}

text_t *scene_get_text(scene_t *scene, size_t i)
{
    return list_get(scene->texts, i);
}

void scene_add_text(scene_t *scene, text_t *text)
{
    if (text)
    {
        list_add(scene->texts, text);
    }
}

void scene_remove_text(scene_t *scene, size_t i)
{
    ((text_t *)list_get(scene->texts, i))->removed = true;
}

size_t scene_images(scene_t *scene)
{
    return list_size(scene->images);
}

image_t *scene_get_image(scene_t *scene, size_t i)
{
    return list_get(scene->images, i);
}

void scene_add_image(scene_t *scene, image_t *image)
{
    list_add(scene->images, image);
}

void scene_remove_image(scene_t *scene, size_t i)
{
    ((image_t *)list_get(scene->images, i))->removed = true;
}

void scene_clear(scene_t *scene)
{
    size_t body_index = 0;
    while (body_index < scene_bodies(scene))
    {
        body_t *body = scene_get_body(scene, body_index);
        body_remove(body);
        body_index++;
    }

    size_t text_index = 0;
    while (text_index < scene_texts(scene))
    {
        text_t *text = scene_get_text(scene, text_index);
        text_remove(text);
        text_index++;
    }

    size_t image_index = 0;
    while (image_index < scene_images(scene))
    {
        image_t *image = scene_get_image(scene, image_index);
        image_remove(image);
        image_index++;
    }

    scene_tick(scene, 0);
}
