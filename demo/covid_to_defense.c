#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <string.h>

#include "list.h"
#include "body.h"
#include "vector.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "forces.h"
#include "path.h"
#include "bullet.h"
#include "collision.h"
#include "virus.h"
#include "tower.h"
#include "shop.h"
#include "bullet.h"
#include "global_body_info.h"
#include "tool.h"
#include "sound.h"

//////////////////////// CONSTS AND CONFIGURATION //////////////////////////////

const int DEMO_WINDOW_WIDTH = 1000;
const int DEMO_WINDOW_HEIGHT = 500;
const double DEMO_PLAYING_WIDTH = 770;  // DEMO_WINDOW_WIDTH - MENU_WIDTH
const double DEMO_PLAYING_HEIGHT = 450; // DEMO_WINDOW_HEIGHT - PLAYER_INFO_HEIGHT
const rgb_color_t CTD_FONT_COLOR = {.r = (float)0.0, .g = (float)0.0, .b = (float)0.0};
const int NUM_LOOPS = 10;

// player
const int INITIAL_MONEY = 150;
const int INITIAL_HEALTH = 200;
const int INITIAL_SCORE = 0;
const int INITIAL_LEVEL = 0;
const int MAX_LEVEL = 30;

// player info area
const double PLAYER_INFO_HEIGHT = 50;
const int PLAYER_INFO_FONT_SIZE = 70;

// walls
const double GAME_WALL_MASS = INFINITY;
const double GAME_WALL_WIDTH = 1.0;
const rgb_color_t GAME_WALL_COLOR = {.r = 0.0, .g = 0.0, .b = 0.0};

// shop/menu
const double MENU_WIDTH = 230;
const rgb_color_t SHOP_MENU_COLOR = {.r = (float)0.60, .g = (float)0.60, .b = (float)0.60};
const rgb_color_t UPGRADE_MENU_COLOR = {.r = (float)0.50, .g = (float)0.50, .b = (float)0.50};
const int UPGRADES_FONT_SIZE = 50;

// viruses
const double VIRUS_SPACING = 15;
const vector_t VIRUS_START_POSITION = {.x = 100, .y = 410};
const vector_t VIRUS_SPEED = {.x = 100, .y = 0};

// mouse
const double MOUSE_RANGE = 2;
const size_t MOUSE_NUM_POINTS = 15;

// next wave button
const vector_t NEXT_WAVE_BUTTON1 = {.x = 800, .y = 450};
const vector_t NEXT_WAVE_BUTTON2 = {.x = 975, .y = 475};
const rgb_color_t NEXT_WAVE_BUTTON_COLOR = {.r = (float)0.70, .g = (float)0.20, .b = (float)0.10};
const double BUTTON_MASS = INFINITY;
const int BUTTON_FONT_SIZE = 100;

// help button
const vector_t HELP_BUTTON1 = {.x = 850, .y = 50};
const vector_t HELP_BUTTON2 = {.x = 925, .y = 75};
const rgb_color_t HELP_BUTTON_COLOR = {.r = (float)0.62, .g = (float)0.52, .b = (float)1.0};
const int HELP_BTN_FONT_SIZE = 90;

// quit button
const vector_t QUIT_BUTTON1 = {.x = 785, .y = 10};
const vector_t QUIT_BUTTON2 = {.x = 875, .y = 35};
const rgb_color_t QUIT_BUTTON_COLOR = {.r = (float)0.57, .b = (float)0.21, .g = (float)0.19};

// back to menu button
const vector_t MENU_BUTTON1 = {.x = 895, .y = 10};
const vector_t MENU_BUTTON2 = {.x = 985, .y = 35};
const rgb_color_t MENU_BUTTON_COLOR = {.r = (float)0.25, .b = (float)0.88, .g = (float)0.51};

// start screen
const double PATH_BUTTON_WIDTH = 150.0;
const double PATH_BUTTON_HEIGHT = 50.0;
const double PATH_BUTTON_SPACING = 50.0;
const vector_t PATH_BUTTON1 = {.x = WINDOW_WIDTH / 2 - (150.0 * 1.5 + 50.0), .y = WINDOW_HEIGHT / 8 - 50.0 / 2};
const vector_t PATH_BUTTON2 = {.x = WINDOW_WIDTH / 2 - (150.0 * .5 + 50.0), .y = WINDOW_HEIGHT / 8 + 50.0 / 2};
const rgb_color_t PATH_BUTTON_COLOR = {.r = (float)0.52, .g = (float)0.91, .b = (float)1};

// game over screen
const double RESTART_BUTTON_WIDTH = 150.0;
const double RESTART_BUTTON_HEIGHT = 50.0;
const vector_t RESTART_BUTTON1 = {.x = WINDOW_WIDTH / 2 - 75.0, .y = WINDOW_HEIGHT / 8 - 25.0};
const vector_t RESTART_BUTTON2 = {.x = WINDOW_WIDTH / 2 + 75.0, .y = WINDOW_HEIGHT / 8 + 25.0};
const double GAMEOVER_SCREEN_RESTART_BTN_OFFSET = 150;
const double GAMEOVER_SCREEN_QUIT_BTN_OFFSET = 200;
const double CONFETTI_ELASTICITY = 1.0;

//////////////////////////// FUNCTION DECLARATIONS //////////////////////////////////

game_state_t initial_game_state(scene_t *scene);
void covid_mouse_handler(mouse_event_type_t type, vector_t mouse_pos, game_state_t *game_state);
void covid_key_handler(char key, key_event_type_t type);
int load_wave(int n, scene_t *scene, game_state_t *game_state);
void create_game_wall(game_state_t *game_state, double x1, double y1, double x2, double y2);
void build_walls(game_state_t *game_state, double window_height, double window_width);
void build_game_walls(game_state_t *game_state);
void update_player_info_text(game_state_t *game_state, char *text);
void play_game_screen(game_state_t *game_state);
int get_high_score(char *filename);
void update_score_txt(char *filename, int new_score);
void welcome_screen(game_state_t *game_state); // 1 (order of start of game events)
void story_screen(game_state_t *game_state);   // 2
void start_screen(game_state_t *game_state);   // 3
void start_game(game_state_t *game_state, draw_path_func_t draw_path);
void game_over_screen(game_state_t *game_state);
void make_simple_img_body(scene_t *scene, vector_t position, vector_t img_size, char *img_path);
void make_quit_button(scene_t *scene, vector_t point1, vector_t point2, rgb_color_t quit_button_color);

///////////////////////////////////// FUNCTIONS ///////////////////////////////////////////

game_state_t initial_game_state(scene_t *scene)
{
    sound_t *pop_sound = sound_init("sounds/pop.wav", 0);
    sound_t *purchase_sound = sound_init("sounds/purchased.wav", 0);
    sound_t *win_sound = sound_init("sounds/win.wav", 0);
    sound_t *lose_sound = sound_init("sounds/lose1.wav", 0);

    return (game_state_t){.scene = scene, .money = INITIAL_MONEY, .health = INITIAL_HEALTH,
        .level = 0, .score = 0, .last_clicked_item_type = NOTHING,
        .last_tower_selected = NULL, .global_shop_item_selected = NOTHING,
        .specific_shop_item_selected = NOT_TOWER, .purchased_item_cost = 0,
        .screen = WELCOME_SCREEN, .pop_sound = pop_sound, .purchase_sound = purchase_sound,
        .win_sound = win_sound, .lose_sound = lose_sound, .shop_description = NULL,
        .help_image = NULL, .virus_count = 0};
}

int load_wave(int n, scene_t *scene, game_state_t *game_state)
{
    // each number in waves.txt represents the health of a virus
    // each line represents one wave of viruses
    // nth line of file is the wave being rendered

    FILE *f = fopen("waves.txt", "r");
    int line_count = 0;
    char line[1000];

    int total_health = 0;

    while (fgets(line, sizeof line, f) != NULL)
    {
        if (line_count == n)
        {
            for (size_t i = 0; i < strlen(line) - 1; i++) // character count
            {
                // converting alphabet to number
                if (line[i] != ' ')
                {
                    int health = line[i] - 'a' + 1;
                    vector_t pos = {VIRUS_START_POSITION.x - VIRUS_SPACING * (i + 1), VIRUS_START_POSITION.y};
                    vector_t speed = VIRUS_SPEED;
                    if (i < strlen(line) - 2)
                    {
                        if (isdigit(line[i + 1]))
                        {
                            speed = vec_multiply(1 + (line[i + 1] - '0') / 10.0, speed);
                            i++;
                        }
                    }
                    if (line[i] == '!')
                    {
                        create_virus(scene, 40, speed, pos, game_state, true);
                        total_health += SUPER_VIRUS_HEALTH + SUPER_VIRUS_SPAWN_COUNT * SUPER_VIRUS_SPAWN_HEALTH;
                    }
                    else if (health > 0 && health <= 26)
                    {
                        create_virus(scene, health, speed, pos, game_state, false);
                        total_health += health;
                    }
                }
            }
            line_count++;
        }
        else
        {
            line_count++;
        }
    }
    fclose(f);
    return total_health;
}

void covid_key_handler(char key, key_event_type_t type)
{
    if (type == KEY_PRESSED)
    {
        switch (key)
        {
        case QUIT:
            printf("Quit game successfully\n");
            exit(1);
        }
    }
}

void covid_mouse_handler(mouse_event_type_t type, vector_t mouse_pos, game_state_t *game_state)
{
    // adjust mouse position for standard window
    mouse_pos = (vector_t){.x = mouse_pos.x, .y = DEMO_WINDOW_HEIGHT - mouse_pos.y};
    if (type == MOUSE_PRESSED)
    {
        list_t *mouse = polygon_make_circle(mouse_pos, MOUSE_RANGE * 2, MOUSE_NUM_POINTS);
        undisplay_help(game_state);
        shop_undisplay_description(game_state);

        if (game_state->last_clicked_item_type == NOTHING)
        {
            for (size_t i = 0; i < scene_bodies(game_state->scene); i++)
            {
                body_t *curr_body = scene_get_body(game_state->scene, i);
                collision_info_t c = find_collision(mouse, body_get_shape(curr_body));
                global_body_type_t type = get_global_type(curr_body);
                if (c.collided)
                {
                    if (type == SHOP_TYPE)
                    {
                        purchase_item(game_state, curr_body);
                        break;
                    }
                    else if (type == NEXT_WAVE_BUTTON_TYPE)
                    {
                        if (game_state->level < MAX_LEVEL)
                        {
                            if (game_state->virus_count == 0) // check if level has been completed
                            {
                                int bonus = load_wave(game_state->level++, game_state->scene, game_state) * game_state->level / (game_state->level);
                                game_state->money += bonus;
                            }
                            else
                            {
                                printf("current level still in progress! \n");
                            }
                        }
                        else
                        {
                            printf("wait until level %d ends \n", game_state->level);
                        }
                        break;
                    }
                    else if (type == BEGIN_BUTTON_TYPE)
                    {
                        story_screen(game_state);
                        break;
                    }
                    else if (type == EASY_PATH_BUTTON_TYPE)
                    {
                        start_game(game_state, draw_complex_path);
                        break;
                    }
                    else if (type == MEDIUM_PATH_BUTTON_TYPE)
                    {
                        start_game(game_state, draw_medium_path);
                        break;
                    }
                    else if (type == HARD_PATH_BUTTON_TYPE)
                    {
                        start_game(game_state, draw_simple_path);
                        break;
                    }
                    else if (type == TOWER_TYPE)
                    {
                        tower_display_range(curr_body, game_state->scene);
                        shop_display_upgrade_button(curr_body, game_state);
                        break;
                    }
                    else if (type == HELP_BUTTON_TYPE)
                    {
                        display_help(game_state);
                        break;
                    }
                    else if (type == MENU_BUTTON_TYPE)
                    {
                        start_screen(game_state);
                        break;
                    }
                    else if (type == RESTART_BUTTON_TYPE)
                    {
                        start_screen(game_state);
                        break;
                    }
                    else if (type == QUIT_BUTTON_TYPE)
                    {
                        printf("Quit game successfully\n");
                        exit(1);
                    }
                }
            }
        }
        else if (game_state->last_clicked_item_type == TOWER_TYPE)
        {
            for (size_t i = 0; i < scene_bodies(game_state->scene); i++)
            {
                body_t *curr_body = scene_get_body(game_state->scene, i);
                collision_info_t c = find_collision(mouse, body_get_shape(curr_body));
                global_body_type_t type = get_global_type(curr_body);
                if (type == UPGRADE_BUTTON_TYPE && c.collided)
                {
                    body_t *tower_body = game_state->last_tower_selected;
                    shop_purchase_tower_upgrade(tower_body, game_state);
                    tower_display_range(tower_body, game_state->scene);
                    break;
                }
            }
            tower_undisplay_range(game_state->scene);
            tower_undisplay_upgrade_button(game_state->scene);
            shop_reset_game_state(game_state, true);
        }
        else if (game_state->last_clicked_item_type == SHOP_TYPE)
        {
            shop_place_item(game_state, mouse_pos, DEMO_PLAYING_WIDTH, DEMO_PLAYING_HEIGHT);
            body_t *b = NULL;
            for (size_t i = 0; i < scene_bodies(game_state->scene); i++)
            {
                body_t *curr_body = scene_get_body(game_state->scene, i);
                if (get_global_type(curr_body) == MOUSE_TYPE)
                {
                    b = curr_body;
                    break;
                }
            }
            if (b != NULL)
            {
                body_remove(b);
            }
        }
        list_free(mouse);
    }
    else
    {
        if (game_state->last_clicked_item_type == SHOP_TYPE)
        {
            shop_temp_display(game_state, mouse_pos, DEMO_PLAYING_WIDTH, DEMO_PLAYING_HEIGHT);
        }
    }
}

void create_game_wall(game_state_t *game_state, double x1, double y1, double x2, double y2)
{
    scene_t *scene = game_state->scene;
    list_t *shape = polygon_make_rectangle(x1, y1, x2, y2);
    body_t *wall = body_init_with_secondary_info(shape, GAME_WALL_MASS, GAME_WALL_COLOR,
                                                 NULL, NULL, WALL_TYPE);
    scene_add_body(scene, wall);
}

void build_walls(game_state_t *game_state, double window_height, double window_width)
{
    double radius = bullet_get_radius();
    // left
    create_game_wall(game_state, -1.0 * radius, 0, 0, window_height);
    // right
    create_game_wall(game_state, window_width, 0,
                     window_width + radius, window_height);
    // top
    create_game_wall(game_state, -1.0 * radius, window_height,
                     window_width + radius, window_height + radius);
    // bottom
    create_game_wall(game_state, -1.0 * radius, -1.0 * radius, window_width + radius, 0);
}

void build_game_walls(game_state_t *game_state)
{
    build_walls(game_state, DEMO_PLAYING_HEIGHT, DEMO_PLAYING_WIDTH);
}

void update_player_info_text(game_state_t *game_state, char *text)
{
    *text = '\0';

    strcat(text, "Health: ");
    char strhealth[20];
    sprintf(strhealth, "%d", game_state->health);
    strcat(text, strhealth);

    strcat(text, ", Level: ");
    char strlvl[10];
    sprintf(strlvl, "%d", game_state->level);
    strcat(text, strlvl);

    strcat(text, ", Money: ");
    char strmoney[10];
    sprintf(strmoney, "%d", game_state->money);
    strcat(text, strmoney);

    strcat(text, ", Score : ");
    char strscore[10];
    sprintf(strscore, "%d", game_state->score);
    strcat(text, strscore);

    strcat(text, ", High Score: ");
    char strhscore[10];
    sprintf(strhscore, "%d", get_high_score("scores.txt"));
    strcat(text, strhscore);
}

void play_game_screen(game_state_t *game_state)
{
    scene_t *scene = game_state->scene;

    // Create shop area
    list_t *shop_menu_background_shape = polygon_make_rectangle(DEMO_WINDOW_WIDTH - MENU_WIDTH,
                                                                DEMO_WINDOW_HEIGHT, DEMO_WINDOW_WIDTH,
                                                                DEMO_WINDOW_HEIGHT);
    body_t *shop_menu = body_init_with_secondary_info(shop_menu_background_shape, INFINITY,
                                                      SHOP_MENU_COLOR, NULL, NULL, NOTHING);
    scene_add_body(scene, shop_menu);

    // Create health/score/level area
    list_t *player_info_background_shape = polygon_make_rectangle(0, DEMO_WINDOW_HEIGHT - PLAYER_INFO_HEIGHT,
                                                                  DEMO_PLAYING_WIDTH, DEMO_WINDOW_HEIGHT);
    global_body_info_t *player_info_global = create_global_body_info(NULL, PLAYER_INFO_TYPE);
    TTF_Font *info_font = create_font("fonts/futura.ttf", PLAYER_INFO_FONT_SIZE);
    vector_t player_info_text_size = (vector_t){DEMO_PLAYING_WIDTH, PLAYER_INFO_HEIGHT};
    char text[100];
    update_player_info_text(game_state, text);

    body_t *player_info_background = body_init_with_info_with_label(player_info_background_shape,
                                                                    BUTTON_MASS, SHOP_MENU_COLOR,
                                                                    player_info_global, NULL,
                                                                    text, CTD_FONT_COLOR, info_font,
                                                                    player_info_text_size);
    scene_add_body(scene, player_info_background);

    // Create next wave/level button
    list_t *button_shape = polygon_make_rectangle(NEXT_WAVE_BUTTON1.x, NEXT_WAVE_BUTTON1.y,
                                                  NEXT_WAVE_BUTTON2.x, NEXT_WAVE_BUTTON2.y);
    global_body_info_t *next_wave_button_global_info = create_global_body_info(NULL, NEXT_WAVE_BUTTON_TYPE);
    TTF_Font *next_wave_button_font = create_font("fonts/airstrike.ttf", BUTTON_FONT_SIZE);
    body_t *next_wave_button = body_init_with_info_with_label(button_shape, BUTTON_MASS,
                                                              NEXT_WAVE_BUTTON_COLOR, next_wave_button_global_info,
                                                              NULL, "NEXT WAVE", CTD_FONT_COLOR, next_wave_button_font,
                                                              (vector_t){NEXT_WAVE_BUTTON2.x - NEXT_WAVE_BUTTON1.x,
                                                                         NEXT_WAVE_BUTTON2.y - NEXT_WAVE_BUTTON1.y});
    scene_add_body(scene, next_wave_button);

    // Create upgrades text area
    TTF_Font *upgrades_font = create_font("fonts/futura.ttf", UPGRADES_FONT_SIZE);
    vector_t divider_position = (vector_t){.x = (DEMO_PLAYING_WIDTH + DEMO_WINDOW_WIDTH) / 2, .y = WINDOW_HEIGHT * 1.4 / 3};
    vector_t divider_size = (vector_t){.x = 230, .y = 10};
    text_t *divider = text_init("_", divider_position, divider_size, CTD_FONT_COLOR, upgrades_font);
    scene_add_text(scene, divider);

    // Create help button
    list_t *button_shape2 = polygon_make_rectangle(HELP_BUTTON1.x, HELP_BUTTON1.y,
                                                   HELP_BUTTON2.x, HELP_BUTTON2.y);
    global_body_info_t *help_button_global_info = create_global_body_info(NULL, HELP_BUTTON_TYPE);
    TTF_Font *help_btn_font = create_font("fonts/TitilliumWeb-SemiBold.ttf", HELP_BTN_FONT_SIZE);
    body_t *help_button = body_init_with_info_with_label(button_shape2, BUTTON_MASS,
                                                         HELP_BUTTON_COLOR, help_button_global_info,
                                                         NULL, "HELP", CTD_FONT_COLOR, help_btn_font,
                                                         (vector_t){HELP_BUTTON2.x - HELP_BUTTON1.x,
                                                                    HELP_BUTTON2.y - HELP_BUTTON1.y});
    scene_add_body(scene, help_button);

    // Create quit game button
    make_quit_button(scene, QUIT_BUTTON1, QUIT_BUTTON2, QUIT_BUTTON_COLOR);

    // Create back to main screen button
    list_t *menu_button_shape = polygon_make_rectangle(MENU_BUTTON1.x, MENU_BUTTON1.y,
                                                       MENU_BUTTON2.x, MENU_BUTTON2.y);
    global_body_info_t *menu_button_global_info = create_global_body_info(NULL, MENU_BUTTON_TYPE);
    TTF_Font *adventure_font = create_font("fonts/adventure.ttf", BUTTON_FONT_SIZE);
    body_t *menu_button = body_init_with_info_with_label(menu_button_shape, BUTTON_MASS,
                                                         MENU_BUTTON_COLOR, menu_button_global_info, NULL,
                                                         "BACK TO MENU", CTD_FONT_COLOR, adventure_font,
                                                         (vector_t){MENU_BUTTON2.x - MENU_BUTTON1.x,
                                                                    MENU_BUTTON2.y - MENU_BUTTON1.y});
    scene_add_body(scene, menu_button);
}

void make_quit_button(scene_t *scene, vector_t point1, vector_t point2, rgb_color_t quit_button_color)
{
    TTF_Font *font = create_font("fonts/adventure.ttf", BUTTON_FONT_SIZE);

    list_t *quit_button_shape = polygon_make_rectangle(point1.x, point1.y, point2.x, point2.y);
    global_body_info_t *quit_button_global_info = create_global_body_info(NULL, QUIT_BUTTON_TYPE);
    body_t *quit_button = body_init_with_info_with_label(quit_button_shape, BUTTON_MASS,
                                                         quit_button_color, quit_button_global_info, NULL,
                                                         "QUIT GAME", CTD_FONT_COLOR, font,
                                                         (vector_t){point2.x - point1.x, point2.y - point1.y});

    scene_add_body(scene, quit_button);
}

int get_high_score(char *filename)
{
    FILE *f = fopen(filename, "r");
    int high_score = 0;

    char line[5]; // 4 bytes to store number + 1 byte for '\n' character
    while (fgets(line, sizeof line, f) != NULL)
    {
        int score = atoi(line);
        if (score > high_score)
        {
            high_score = score;
        }
    }
    fclose(f);
    return high_score;
}

void update_score_txt(char *filename, int new_score)
{
    FILE *f = fopen(filename, "a");
    fprintf(f, "%d\n", new_score);
    fclose(f);
}

void make_simple_img_body(scene_t *scene, vector_t position, vector_t img_size, char *img_path)
{
    size_t num_points = 5;
    double mass = 10.0;
    rgb_color_t color = WHITE;
    double radius = 5.0;

    list_t *shape = polygon_make_circle(position, radius, num_points);
    body_t *image_body = body_init(shape, mass, color);
    image_t *image = image_init(img_path, position, img_size, IMG_INIT_PNG);

    body_set_image(image_body, image);
    scene_add_body(scene, image_body);
}

// HAPPENS 1ST
// INTRO PAGE (VERY FIRST PAGE):
void welcome_screen(game_state_t *game_state)
{
    scene_t *scene = game_state->scene;
    *game_state = initial_game_state(scene);
    scene_clear(scene);
    game_state->screen = WELCOME_SCREEN;

    // title
    vector_t position = vec_add((vector_t){WINDOW_WIDTH / 2, WINDOW_HEIGHT * 2 / 3}, (vector_t){0, 70});
    vector_t size = (vector_t){WINDOW_WIDTH - 200, WINDOW_HEIGHT - 400};
    TTF_Font *game_title_font = create_font("fonts/airstrike.ttf", 130);
    rgb_color_t title_font_color = (rgb_color_t){(float)0.6, (float)0.96, (float)0.7};
    text_t *title = text_init("COVID TO DEFENSE", position, size, title_font_color, game_title_font);
    scene_add_text(scene, title);

    // images
    vector_t img1_pos = (vector_t){170, WINDOW_HEIGHT / 2};
    vector_t img2_pos = vec_add(img1_pos, (vector_t){WINDOW_WIDTH / 3, 0});
    vector_t img3_pos = vec_add(img2_pos, (vector_t){WINDOW_WIDTH / 3, 0});
    vector_t img_size = (vector_t){250, 150};

    make_simple_img_body(game_state->scene, img1_pos, img_size, "images/start-screen-1.png");
    make_simple_img_body(game_state->scene, img2_pos, img_size, "images/start-screen-2.png");
    make_simple_img_body(game_state->scene, img3_pos, img_size, "images/start-screen-3.png");

    // button: Click here to begin!
    vector_t bottom_corner = (vector_t){650, 50};
    vector_t top_corner = (vector_t){350, 150};
    list_t *begin_game_btn_shape = polygon_make_rectangle(bottom_corner.x, bottom_corner.y, top_corner.x, top_corner.y);
    global_body_info_t *begin_btn_global_info = create_global_body_info(NULL, BEGIN_BUTTON_TYPE);
    TTF_Font *begin_font = create_font("fonts/MontereyFLF.ttf", BUTTON_FONT_SIZE);
    vector_t begin_btn_text_size = (vector_t){300, 100};
    rgb_color_t begin_game_btn_color = (rgb_color_t){(float)0.53, (float)0.92, (float)0.8};
    body_t *begin_game_btn = body_init_with_info_with_label(begin_game_btn_shape, BUTTON_MASS, begin_game_btn_color,
                                                            begin_btn_global_info, NULL, "Click here to begin!",
                                                            CTD_FONT_COLOR, begin_font, begin_btn_text_size);
    scene_add_body(scene, begin_game_btn);
}

// HAPPENDS 2ND
// GAME STORYLINE
void story_screen(game_state_t *game_state)
{
    // BUTTON TO PLAY/START GAME (choose paths)
    // when button to play is clicked, start screen is called

    scene_t *scene = game_state->scene;
    *game_state = initial_game_state(scene);
    scene_clear(scene);
    game_state->screen = STORY_SCREEN;

    // continue to main menu button
    TTF_Font *font = create_font("fonts/HighlandGothicFLF.ttf", 150);
    TTF_Font *font2 = create_font("fonts/CelticHand.ttf", 150);
    TTF_Font *font3 = create_font("fonts/MotionPicture.ttf", 150);
    TTF_Font *font4 = create_font("fonts/futura.ttf", 150);

    // first note
    vector_t position1 = (vector_t){330, 470};
    vector_t size1 = (vector_t){620, 55};
    text_t *note1 = text_init("Welcome to the unexpected ride the year 2020 brings...",
                              position1, size1, BLACK, font);
    scene_add_text(scene, note1);

    // second note
    vector_t position2 = (vector_t){500, 400};
    vector_t size2 = (vector_t){780, 40};
    text_t *note2 = text_init("...an unexpected ride beyond the wildest of imaginations...",
                              position2, size2, BLACK, font);
    scene_add_text(scene, note2);

    // Third note
    vector_t position3 = (vector_t){480, 355};
    vector_t size3 = (vector_t){600, 35};
    text_t *note3 = text_init("no words can fully describe the experience", position3, size3, BLACK, font2); // center
    scene_add_text(scene, note3);

    // fourth note
    vector_t position4 = (vector_t){500, 320};
    vector_t size4 = (vector_t){300, 30};
    text_t *note4 = text_init("so it is your duty", position4, size4, BLACK, font);
    scene_add_text(scene, note4);

    // fifth note
    vector_t position5 = (vector_t){500, 280};
    vector_t size5 = (vector_t){150, 40};
    text_t *note5 = text_init("fellow humans", position5, size5, BLACK, font3);
    scene_add_text(scene, note5);

    // sixth note
    vector_t position6 = (vector_t){550, 250};
    vector_t size6 = (vector_t){570, 45};
    text_t *note6 = text_init("to save humankind from another pandemic", position6, size6, BLACK, font);
    scene_add_text(scene, note6);

    // seventh note
    vector_t position7 = (vector_t){450, 190};
    vector_t size7 = (vector_t){640, 65};
    text_t *note7 = text_init("Kill all the coronaviruses before they leak this cyberspace!",
                              position7, size7, BLACK, font);
    scene_add_text(scene, note7);

    // eighth note
    vector_t position8 = (vector_t){230, 40};
    vector_t size8 = (vector_t){390, 25};
    text_t *note8 = text_init("to quit game, use \"quit\" or press \"q\" on your keyboard anytime",
                              position8, size8, BLACK, font4);
    scene_add_text(scene, note8);

    // continue to main menu button
    TTF_Font *adventure_font = create_font("fonts/adventure.ttf", BUTTON_FONT_SIZE);
    list_t *continue_button_shape = polygon_make_rectangle(500, 60, 700, 110);
    global_body_info_t *continue_button_global_info = create_global_body_info(NULL, MENU_BUTTON_TYPE);
    body_t *continue_button = body_init_with_info_with_label(continue_button_shape, BUTTON_MASS,
                                                             (rgb_color_t){(float)1.0, (float)0.44, (float)0.32},
                                                             continue_button_global_info, NULL,
                                                             "CONTINUE", CTD_FONT_COLOR, adventure_font,
                                                             (vector_t){RESTART_BUTTON2.x - RESTART_BUTTON1.x,
                                                                        RESTART_BUTTON2.y - RESTART_BUTTON1.y});
    scene_add_body(scene, continue_button);

    // quit button
    make_quit_button(scene, (vector_t){750, 60}, (vector_t){950, 110}, (rgb_color_t){(float)1.0, (float)0.44, (float)0.32});
}

// happens 3rd
void start_screen(game_state_t *game_state)
{
    scene_t *scene = game_state->scene;
    *game_state = initial_game_state(scene);
    scene_clear(scene);
    game_state->screen = START_GAME_SCREEN;

    vector_t position = vec_add((vector_t){.x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT * 2 / 3}, (vector_t){.x = 0, .y = 70});
    vector_t size = (vector_t){.x = WINDOW_WIDTH - 200, .y = WINDOW_HEIGHT - 400};
    TTF_Font *game_title_font = create_font("fonts/airstrike.ttf", 130);
    text_t *title = text_init("COVID TO DEFENSE", position, size, CTD_FONT_COLOR, game_title_font);
    scene_add_text(scene, title);

    // game screen image --  "Covid to Defense"
    vector_t pos = (vector_t){WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
    make_simple_img_body(scene, pos, (vector_t){550, 150}, "images/c2d.png");

    // create easy path
    vector_t bottom_corner = PATH_BUTTON1;
    vector_t top_corner = PATH_BUTTON2;
    vector_t path_text_size = (vector_t){PATH_BUTTON2.x - PATH_BUTTON1.x, PATH_BUTTON2.y - PATH_BUTTON1.y};
    list_t *easy_button_shape = polygon_make_rectangle(bottom_corner.x, bottom_corner.y,
                                                       top_corner.x, top_corner.y);
    global_body_info_t *easy_path_button_global_info = create_global_body_info(NULL, EASY_PATH_BUTTON_TYPE);
    TTF_Font *paths_font = create_font("fonts/MontereyFLF.ttf", BUTTON_FONT_SIZE);
    body_t *easy_button = body_init_with_info_with_label(easy_button_shape, BUTTON_MASS,
                                                         PATH_BUTTON_COLOR, easy_path_button_global_info, NULL,
                                                         "EASY PATH", CTD_FONT_COLOR, paths_font, path_text_size);
    scene_add_body(scene, easy_button);

    // create medium path
    bottom_corner.x += PATH_BUTTON_WIDTH + PATH_BUTTON_SPACING;
    top_corner.x += PATH_BUTTON_WIDTH + PATH_BUTTON_SPACING;
    list_t *medium_button_shape = polygon_make_rectangle(bottom_corner.x - 10, bottom_corner.y,
                                                         top_corner.x + 10, top_corner.y);
    global_body_info_t *med_path_button_global_info = create_global_body_info(NULL, MEDIUM_PATH_BUTTON_TYPE);
    body_t *medium_button = body_init_with_info_with_label(medium_button_shape, BUTTON_MASS,
                                                           PATH_BUTTON_COLOR, med_path_button_global_info, NULL,
                                                           "MEDIUM PATH", CTD_FONT_COLOR, paths_font, path_text_size);
    scene_add_body(scene, medium_button);

    // create hard path
    bottom_corner.x += PATH_BUTTON_WIDTH + PATH_BUTTON_SPACING;
    top_corner.x += PATH_BUTTON_WIDTH + PATH_BUTTON_SPACING;
    list_t *hard_button_shape = polygon_make_rectangle(bottom_corner.x, bottom_corner.y,
                                                       top_corner.x, top_corner.y);
    global_body_info_t *hard_path_button_global_info = create_global_body_info(NULL, HARD_PATH_BUTTON_TYPE);
    body_t *hard_button = body_init_with_info_with_label(hard_button_shape, BUTTON_MASS,
                                                         PATH_BUTTON_COLOR, hard_path_button_global_info, NULL,
                                                         "HARD PATH", CTD_FONT_COLOR, paths_font, path_text_size);
    scene_add_body(scene, hard_button);
}

void start_game(game_state_t *game_state, draw_path_func_t draw_path)
{
    game_state->screen = PLAYING_SCREEN;
    scene_t *scene = game_state->scene;
    scene_clear(scene);
    setup_tower_configs();
    play_game_screen(game_state);
    create_shop(scene);
    draw_path(scene, DEMO_WINDOW_HEIGHT);
    build_game_walls(game_state);
}

void game_over_screen(game_state_t *game_state)
{
    // pause 1 second so not super jarring
    sdl_delay(1);

    scene_t *scene = game_state->scene;
    scene_clear(scene);

    build_walls(game_state, DEMO_WINDOW_HEIGHT, DEMO_WINDOW_WIDTH);

    // win/lose message
    vector_t end_msg_position= (vector_t){.x = 0.5 * WINDOW_WIDTH, .y = 0.65 * WINDOW_HEIGHT};
    vector_t end_msg_size = (vector_t){.x = WINDOW_WIDTH - 500, .y = WINDOW_HEIGHT - 400};
    TTF_Font *end_msg_font = create_font("fonts/CelticHand.ttf", 100);
    if (game_state->screen == WIN_SCREEN)
    {
        // background confetti -- bouncing stars
        text_t *message = text_init("YOU WIN!", end_msg_position, end_msg_size, CTD_FONT_COLOR, end_msg_font);
        scene_add_text(scene, message);

        for (int i = 0; i <= DEMO_WINDOW_WIDTH; i+=DEMO_WINDOW_WIDTH/50)
        {
            vector_t position = (vector_t){.x = DEMO_WINDOW_WIDTH / 2.0, .y = DEMO_WINDOW_HEIGHT / 2.0};
            vector_t velocity = (vector_t){.x = rand() % 500 - 250, .y = rand() % 500 - 250};
            list_t *shape = polygon_make_star(position, rand() % 15 + 10, rand() % 10 + 5);
            body_t *confetti = body_init(shape, 10, color_rainbow(rand() % 50));
            body_set_velocity(confetti, velocity);
            scene_add_body(game_state->scene, confetti);
            for (size_t j = 0; j < scene_bodies(game_state->scene); j++)
            {
                body_t *curr_body = scene_get_body(game_state->scene, j);
                if (get_global_type(curr_body) == WALL_TYPE)
                {
                    create_physics_collision(game_state->scene, CONFETTI_ELASTICITY, confetti, curr_body);
                }
            }
        }
    }
    else if (game_state->screen == LOSE_SCREEN)
    {
        text_t *message = text_init("YOU LOST :(", end_msg_position, end_msg_size, CTD_FONT_COLOR, end_msg_font);
        scene_add_text(scene, message);
        for (int i = 0; i <= DEMO_WINDOW_WIDTH; i+=DEMO_WINDOW_WIDTH/20)
        {
            vector_t position = {.x = i, .y = DEMO_WINDOW_HEIGHT-10};
            vector_t velocity = (vector_t){.x = 0, .y = -50};
            create_virus(game_state->scene, 10, velocity, position, game_state, true);
        }
    }

    // your score string
    vector_t position = (vector_t){.x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT * 2 / 3 - 100};
    vector_t size = (vector_t){.x = WINDOW_WIDTH - 100, .y = WINDOW_HEIGHT - 450};
    TTF_Font *font = create_font("fonts/CelticHand.ttf", 100);
    char *score_text = malloc(sizeof(char) * 50);
    *score_text = '\0';
    strcat(score_text, "YOUR SCORE: ");
    char strscore[10];
    sprintf(strscore, "%d", game_state->score);
    strcat(score_text, strscore);

    // high score string
    strcat(score_text, ", HIGH SCORE: ");
    char strhscore[10];
    sprintf(strhscore, "%d", get_high_score("scores.txt"));
    strcat(score_text, strhscore);

    // render scores
    text_t *scores = text_init(score_text, position, size, CTD_FONT_COLOR, font);
    scene_add_text(scene, scores);

    // restart button
    list_t *restart_button_shape = polygon_make_rectangle(RESTART_BUTTON1.x - GAMEOVER_SCREEN_RESTART_BTN_OFFSET,
                                                          RESTART_BUTTON1.y,
                                                          RESTART_BUTTON2.x - GAMEOVER_SCREEN_RESTART_BTN_OFFSET,
                                                          RESTART_BUTTON2.y);
    global_body_info_t *info = create_global_body_info(NULL, RESTART_BUTTON_TYPE);
    TTF_Font *adventure_font = create_font("fonts/adventure.ttf", BUTTON_FONT_SIZE);
    body_t *restart_button = body_init_with_info_with_label(restart_button_shape, BUTTON_MASS, PATH_BUTTON_COLOR,
                                                            info, NULL, "RESTART", CTD_FONT_COLOR, adventure_font,
                                                            (vector_t){RESTART_BUTTON2.x - RESTART_BUTTON1.x,
                                                                       RESTART_BUTTON2.y - RESTART_BUTTON1.y});
    scene_add_body(scene, restart_button);

    // quit button
    vector_t point1 = (vector_t){RESTART_BUTTON1.x + GAMEOVER_SCREEN_QUIT_BTN_OFFSET, RESTART_BUTTON1.y};
    vector_t point2 = (vector_t){RESTART_BUTTON2.x + GAMEOVER_SCREEN_QUIT_BTN_OFFSET, RESTART_BUTTON2.y};
    make_quit_button(scene, point1, point2, PATH_BUTTON_COLOR);
}

int main(int argc, char *argv[])
{
    srand(42);

    sdl_init(VEC_ZERO, (vector_t){.x = DEMO_WINDOW_WIDTH, .y = DEMO_WINDOW_HEIGHT});
    sdl_on_mouse((mouse_handler_t)covid_mouse_handler);
    sdl_on_key((key_handler_t)covid_key_handler);

    sound_t *background_music = sound_init("sounds/quietcreepybackgroundmusic.wav", NUM_LOOPS);
    sound_play(background_music);

    scene_t *scene = scene_init();
    game_state_t *game_state = malloc(sizeof(game_state_t));
    *game_state = initial_game_state(scene);
    welcome_screen(game_state); // game starts with welcome, then story, then start_screen

    while (!sdl_is_done(game_state))
    {
        if (game_state->screen == PLAYING_SCREEN)
        {
            // update
            int virus_count = 0;
            for (size_t i = 0; i < scene_bodies(scene); i++)
            {
                body_t *curr_body = scene_get_body(scene, i);
                if (get_global_type(curr_body) == VIRUS_TYPE)
                {
                    virus_count++;
                    virus_tick(curr_body);
                }
                else if (get_global_type(curr_body) == BULLET_TYPE)
                {
                    bullet_tick(curr_body);
                }
                else if (get_global_type(curr_body) == PLAYER_INFO_TYPE)
                {
                    char *player_txt = body_get_label(curr_body)->txt;
                    update_player_info_text(game_state, player_txt);
                }
                else if (get_global_type(curr_body) == TOWER_TYPE)
                {
                    tower_tick(curr_body, scene, game_state);
                }
            }
            game_state->virus_count = virus_count;

            if (game_state->level >= MAX_LEVEL && virus_count == 0)
            {
                update_score_txt("scores.txt", game_state->score);
                game_state->screen = WIN_SCREEN;
                sound_play(game_state->win_sound);
                game_over_screen(game_state);
            }
            else if (game_state->health <= 0)
            {
                update_score_txt("scores.txt", game_state->score);
                game_state->screen = LOSE_SCREEN;
                sound_play(game_state->lose_sound);
                game_over_screen(game_state);
            }
        }

        // GAME SCENE INTERVAL
        double dt = time_since_last_tick();
        scene_tick(game_state->scene, dt);
        sdl_render_scene(game_state->scene);
    }

    // CLEANUP
    scene_free(scene);
    return 1;
}
