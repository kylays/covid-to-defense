#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "sdl_wrapper.h"

const char WINDOW_TITLE[] = "CS 3";
const double MS_PER_S = 1e3;

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;
/**
 * The mouse handler, or NULL if none has been configured.
 */
mouse_handler_t mouse_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

// private function declarations
void sdl_render_image(char *image_name, vector_t coord, vector_t size, int image_type);
void sdl_render_text(const char *text, vector_t coord, vector_t size, rgb_color_t colour, TTF_Font *font);
void my_audio_callback(void *userdata, Uint8 *stream, int len);
void sdl_play_music(char *filename);

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void)
{
    int *width = malloc(sizeof(*width)),
        *height = malloc(sizeof(*height));
    assert(width != NULL);
    assert(height != NULL);
    SDL_GetWindowSize(window, width, height);
    vector_t dimensions = {.x = *width, .y = *height};
    free(width);
    free(height);
    return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center)
{
    // Scale scene so it fits entirely in the window
    double x_scale = window_center.x / max_diff.x,
           y_scale = window_center.y / max_diff.y;
    return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center)
{
    // Scale scene coordinates by the scaling factor
    // and map the center of the scene to the center of the window
    vector_t scene_center_offset = vec_subtract(scene_pos, center);
    double scale = get_scene_scale(window_center);
    vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
    vector_t pixel = {
        .x = round(window_center.x + pixel_center_offset.x),
        // Flip y axis since positive y is down on the screen
        .y = round(window_center.y - pixel_center_offset.y)};
    return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_LEFT:
        return LEFT_ARROW;
    case SDLK_UP:
        return UP_ARROW;
    case SDLK_RIGHT:
        return RIGHT_ARROW;
    case SDLK_DOWN:
        return DOWN_ARROW;
    case SDLK_ESCAPE:
        return ESCAPE;
    case SDLK_q:
        return QUIT;
    default:
        // Only process 7-bit ASCII characters
        return key == (SDL_Keycode)(char)key ? key : '\0';
    }
}

void sdl_init(vector_t min, vector_t max)
{
    // Check parameters
    assert(min.x < max.x);
    assert(min.y < max.y);

    center = vec_multiply(0.5, vec_add(min, max));
    max_diff = vec_subtract(max, center);
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);

    TTF_Init();

    // Initialize sound
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        assert(true == false);
    }
}

bool sdl_is_done(void *object)
{
    SDL_Event *event = malloc(sizeof(*event));
    assert(event != NULL);
    while (SDL_PollEvent(event))
    {
        switch (event->type)
        {
        case SDL_QUIT:
            free(event);
            return true;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            // Skip the keypress if no handler is configured
            // or an unrecognized key was pressed
            if (key_handler == NULL)
                break;
            char key = get_keycode(event->key.keysym.sym);
            if (key == '\0')
                break;

            uint32_t timestamp = event->key.timestamp;
            if (!event->key.repeat)
            {
                key_start_timestamp = timestamp;
            }
            key_event_type_t ktype =
                event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
            double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
            key_handler(key, ktype, held_time, object);
            break;
        }
        if (mouse_handler == NULL)
            break;
        mouse_event_type_t mtype =
            event->type == SDL_MOUSEBUTTONDOWN ? MOUSE_PRESSED : SDL_MOUSEBUTTONUP ? MOUSE_RELEASED
                                                                                   : MOUSE_MOTION;
        vector_t mouse_pos = {.x = event->motion.x, .y = event->motion.y};
        mouse_handler(mtype, mouse_pos, object);
        break;
    }
    free(event);
    return false;
}

void sdl_clear(void)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void sdl_draw_polygon(list_t *points, rgb_color_t color)
{
    // Check parameters
    size_t n = list_size(points);
    assert(n >= 3);
    assert(0 <= color.r && color.r <= 1);
    assert(0 <= color.g && color.g <= 1);
    assert(0 <= color.b && color.b <= 1);

    vector_t window_center = get_window_center();

    // Convert each vertex to a point on screen
    int16_t *x_points = malloc(sizeof(*x_points) * n),
            *y_points = malloc(sizeof(*y_points) * n);
    assert(x_points != NULL);
    assert(y_points != NULL);
    for (size_t i = 0; i < n; i++)
    {
        vector_t *vertex = list_get(points, i);
        vector_t pixel = get_window_position(*vertex, window_center);
        x_points[i] = (int16_t)pixel.x;
        y_points[i] = (int16_t)pixel.y;
    }

    // Draw polygon with the given color
    filledPolygonRGBA(
        renderer,
        x_points, y_points, (int)n,
        (Uint8)(color.r * 255), (Uint8)(color.g * 255), (Uint8)(color.b * 255), 255);
    free(x_points);
    free(y_points);
}

void sdl_show(void)
{
    // Draw boundary lines
    vector_t window_center = get_window_center();
    vector_t max = vec_add(center, max_diff),
             min = vec_subtract(center, max_diff);
    vector_t max_pixel = get_window_position(max, window_center),
             min_pixel = get_window_position(min, window_center);
    SDL_Rect *boundary = malloc(sizeof(*boundary));
    boundary->x = (int)min_pixel.x;
    boundary->y = (int)max_pixel.y;
    boundary->w = (int)max_pixel.x - (int)min_pixel.x;
    boundary->h = (int)min_pixel.y - (int)max_pixel.y;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, boundary);
    free(boundary);

    SDL_RenderPresent(renderer);
}

void sdl_render_scene(scene_t *scene)
{
    sdl_clear();

    size_t body_count = scene_bodies(scene);
    for (size_t i = 0; i < body_count; i++)
    {
        body_t *body = scene_get_body(scene, i);
        list_t *shape = body_get_shape(body);
        sdl_draw_polygon(shape, body_get_color(body));
        list_free(shape);
    }

    size_t text_count = scene_texts(scene);
    for (size_t i = 0; i < text_count; i++)
    {
        text_t *text = scene_get_text(scene, i);
        sdl_render_text(text->txt, text->position, text->size, text->color, text->font);
    }

    size_t image_count = scene_images(scene);
    for (size_t i = 0; i < image_count; i++)
    {
        image_t *image = scene_get_image(scene, i);
        sdl_render_image(image->image_filename, image->position, image->size, image->image_type);
    }

    sdl_show();
}

void sdl_on_key(key_handler_t handler)
{
    key_handler = handler;
}

void sdl_on_mouse(mouse_handler_t handler)
{
    mouse_handler = handler;
}

double time_since_last_tick(void)
{
    clock_t now = clock();
    double difference = last_clock
                            ? (double)(now - last_clock) / CLOCKS_PER_SEC
                            : 0.0; // return 0 the first time this is called
    last_clock = now;
    return difference;
}

void sdl_delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
    clock_t start_time = clock();
    while (clock() < start_time + milli_seconds)
        ;
}

// Modified from https://stackoverflow.com/questions/22852226/c-sdl2-how-to-regularly-update-a-renderered-text-ttf
void sdl_render_text(const char *text, vector_t coord, vector_t size, rgb_color_t colour, TTF_Font *font)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Color color;
    SDL_Rect rect;
    color.r = (int)(colour.r * 255); // (int) colour.r * 255;
    color.g = (int)(colour.g * 255);
    color.b = (int)(colour.b * 255);
    color.a = 0;

    // assert(font != NULL);
    surface = TTF_RenderText_Solid(font, text, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect.x = (int)(coord.x - size.x / 2);
    rect.y = WINDOW_HEIGHT - (int)(coord.y + size.y / 2);
    rect.w = (int)size.x;
    rect.h = (int)size.y;
    /* This is wasteful for textures that stay the same.
     * But makes things less stateful and easier to use.
     * Not going to code an atlas solution here... are we? */
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

TTF_Font *create_font(const char *font_path, int font_size)
{
    TTF_Font *font = TTF_OpenFont(font_path, font_size);
    if (font)
    {
        return TTF_OpenFont(font_path, font_size);
    }
    // if font is NULL
    return TTF_OpenFont("fonts/VertigoFLF.ttf", font_size); // default font
}

void sdl_cleanup()
{
    IMG_Quit();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdl_render_image(char *image_name, vector_t coord, vector_t size, int image_type)
{
    IMG_Init(image_type);
    SDL_Surface *surface = IMG_Load(image_name);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    //'rect' defines the dimensions of the rendering sprite on window
    SDL_Rect rect;
    rect.x = (int)(coord.x - size.x / 2);
    rect.y = WINDOW_HEIGHT - (int)(coord.y + size.y / 2);
    rect.w = (int)size.x;
    rect.h = (int)size.y;
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
}

// Music Code Source: https://gist.github.com/armornick/3447121
static Uint8 *audio_pos;
static Uint32 audio_len;

void sdl_play_music(char *filename)
{
    // plays music for duration of sdl window

    static Uint32 wav_length;      // length of our sample
    static Uint8 *wav_buffer;      // buffer containing our audio file
    static SDL_AudioSpec wav_spec; // the specs of our piece of music

    /* Load the WAV */
    // the specs, length and buffer of our wav are filled
    assert(SDL_LoadWAV(filename, &wav_spec, &wav_buffer, &wav_length) != NULL);

    // set the callback function
    wav_spec.callback = my_audio_callback;
    wav_spec.userdata = NULL;
    // set our global static variables
    audio_pos = wav_buffer; // copy sound buffer
    audio_len = wav_length; // copy file length

    /* Open the audio device */
    if (SDL_OpenAudio(&wav_spec, NULL) < 0)
    {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        exit(-1);
    }

    /* Start playing */
    SDL_PauseAudio(0);
}

void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
    if (audio_len == 0)
        return;

    len = (len > (int)audio_len ? audio_len : len);
    SDL_memcpy(stream, audio_pos, len);                      // simply copy from one buffer into the other
    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME); // mix from one buffer into another

    audio_pos += len;
    audio_len -= len;
}
