#include "shop.h"
#include "tower.h"
#include "tool.h"
#include "global_body_info.h"
#include "path.h"

const vector_t SHOP_START = {.x = 805, .y = 375}; // keep these dimensions ints for mod math later
const size_t ITEM_SPACING = 15;
const double ITEM_MASS = INFINITY;
const int SHOP_WIDTH = 300;
const int SHOP_HEIGHT = 1000;
const size_t SHOP_TOWER_CIRCLE_POINTS = 10;
const double UPGRADE_BUTTON_HEIGHT = 25;
const double UPGRADE_BUTTON_WIDTH = 175;
const rgb_color_t PASTEL_GREEN = {.r = (float)0.7, .g = (float)1.0, .b = (float)0.67};
const rgb_color_t PASTEL_RED = {.r = (float)1, .g = (float)0.5, .b = (float)0.5};
const rgb_color_t SHOP_TEXT_COLOR = {.r = (float)0, .g = (float)0, .b = (float)0};
const int SHOP_BUTTON_FONT_SIZE = 50;
const vector_t DESCRIPTION_SIZE = {.x = 210, .y = 20};
const vector_t DESCRIPTION_POSITION = {.x = 887, .y = 167};
const vector_t HELP_SIZE = {.x = 975, .y = 475};
const vector_t HELP_POSITION = {.x = 500, .y = 250};
const vector_t CAMO_NOTE_POSITION = {.x = 887, .y = 250};
const vector_t CAMO_NOTE_SIZE = {.x = 210, .y = 25};

typedef struct shop_item
{
    int cost;
    global_body_type_t global_type;
    int specific_type;
    char *description;
} shop_item_t;

// private function declarations
char *shop_price_label(int cost);
bool is_within_path(body_t *path_body, vector_t mouse_pos, scene_t *scene, double radius);
bool is_within_window(vector_t mouse_pos, double demo_playing_width, double demo_playing_height, double radius);
bool overlap_with_towers(vector_t mouse_pos, scene_t *scene, double radius);

shop_item_t *shop_item_init(int cost, global_body_type_t global_type, int specific_type, char *description)
{
    shop_item_t *s = malloc(sizeof(shop_item_t));
    assert(s != NULL);
    s->cost = cost;
    s->global_type = global_type;
    s->specific_type = specific_type;
    s->description = description;
    return s;
}

char *shop_price_label(int cost)
{
    char *price_label = malloc(sizeof(char) * 10);
    *price_label = '\0';
    strcat(price_label, "$");
    char cost_str[5];
    sprintf(cost_str, "%d", cost);
    strcat(price_label, cost_str);
    return price_label;
}

body_t *create_shop_item_body(scene_t *scene, global_body_type_t global_type, int specific_type, vector_t position)
{
    if (global_type == TOWER_TYPE)
    {
        shop_item_t *shop_item_info = shop_item_init(tower_get_from_id(specific_type).price, global_type, specific_type, tower_get_description_from_id(specific_type));
        list_t *shop_item_shape = polygon_make_circle(position, tower_get_radius(), SHOP_TOWER_CIRCLE_POINTS);
        char *price_label = shop_price_label(shop_item_info->cost);
        if(specific_type % 3 == 1 || specific_type == TACK_SHOOTER_ID){
            strcat(price_label, "*");
        }
        vector_t size = {35, 30};
        TTF_Font *font = create_font("fonts/futura.ttf", SHOP_BUTTON_FONT_SIZE);
        global_body_info_t *info = create_global_body_info(shop_item_info, SHOP_TYPE);
        body_t *shop_item_body = body_init_with_info_with_label(shop_item_shape, ITEM_MASS, tower_get_color(specific_type), info, free, price_label, SHOP_TEXT_COLOR, font, size);
        scene_add_body(scene, shop_item_body);
        free(price_label); // free because text_init mallocs again
        return shop_item_body;
    }
    else if (global_type == TOOL_TYPE)
    {
        shop_item_t *shop_item_info = shop_item_init(tool_get_from_id(specific_type).price, global_type, specific_type, tool_get_description_from_id(specific_type));
        list_t *shop_item_shape = polygon_make_square(position, tool_get_radius());
        char *price_label = shop_price_label(shop_item_info->cost);
        vector_t size = {25, 25};
        TTF_Font *font = create_font("fonts/futura.ttf", SHOP_BUTTON_FONT_SIZE);
        global_body_info_t *info = create_global_body_info(shop_item_info, SHOP_TYPE);
        body_t *shop_item_body = body_init_with_info_with_label(shop_item_shape, ITEM_MASS, tool_get_from_id(specific_type).color, info, free, price_label, SHOP_TEXT_COLOR, font, size);
        scene_add_body(scene, shop_item_body);
        free(price_label); // free because text_init mallocs again
        return shop_item_body;
    }
    return NULL;
}

void create_shop(scene_t *scene)
{
    // TOP TOWER ROW
    vector_t pos = vec_add(SHOP_START, (vector_t){.x = 0, .y = 20});
    create_shop_item_body(scene, TOWER_TYPE, DART_TOWER_ID, pos);

    pos = (vector_t){.x = pos.x + (ITEM_SPACING + 2 * tower_get_radius()), .y = pos.y};
    create_shop_item_body(scene, TOWER_TYPE, TACK_SHOOTER_ID, pos);

    pos = (vector_t){.x = pos.x + (ITEM_SPACING + 2 * tower_get_radius()), .y = pos.y};
    create_shop_item_body(scene, TOWER_TYPE, BOMB_SHOOTER_ID, pos);

    pos = (vector_t){.x = pos.x + (ITEM_SPACING + 2 * tower_get_radius()), .y = pos.y};
    create_shop_item_body(scene, TOWER_TYPE, BOOMERANG_ID, pos);

    // BOTTOM TOWER ROW
    pos = (vector_t){.x = SHOP_START.x, .y = pos.y - (ITEM_SPACING + 2 * tower_get_radius())};
    create_shop_item_body(scene, TOWER_TYPE, GLUE_TOWER_ID, pos);

    pos = (vector_t){.x = pos.x + (ITEM_SPACING + 2 * tower_get_radius()), .y = pos.y};
    create_shop_item_body(scene, TOWER_TYPE, SUPER_TOWER_ID, pos);

    pos = (vector_t){.x = pos.x + (ITEM_SPACING + 2 * tower_get_radius()), .y = pos.y};
    create_shop_item_body(scene, TOWER_TYPE, AIRPLANE_ID, pos);

    pos = (vector_t){.x = pos.x + (ITEM_SPACING + 2 * tower_get_radius()), .y = pos.y};
    create_shop_item_body(scene, TOWER_TYPE, BANK_ID, pos);

    // TOOL ROW
    pos = (vector_t){.x = SHOP_START.x, .y = pos.y - 1.5 * (ITEM_SPACING + 2 * tool_get_radius())};
    create_shop_item_body(scene, TOOL_TYPE, (tool_id_t)MASK, pos);

    pos = (vector_t){.x = pos.x + (ITEM_SPACING + 2 * tower_get_radius()), .y = pos.y};
    create_shop_item_body(scene, TOOL_TYPE, (tool_id_t)SANITIZER, pos);

    // display note about camouflaged viruses
    TTF_Font *font = create_font("fonts/futura.ttf", 100);
    text_t *note = text_init("* tower sees camo viruses (black)", CAMO_NOTE_POSITION, CAMO_NOTE_SIZE, SHOP_TEXT_COLOR, font);
    scene_add_text(scene, note);
}

int purchase_item(game_state_t *game_state, body_t *shop_item_body)
{
    // check if the user has enough money
    // if the user does, then set the game state's last clicked to the shop item

    shop_item_t *item = (shop_item_t *)get_global_secondary_info(shop_item_body);
    int price = (item->cost);
    shop_display_description(game_state, shop_item_body);

    if (price <= game_state->money)
    {
        game_state->last_clicked_item_type = SHOP_TYPE;
        game_state->global_shop_item_selected = item->global_type;
        game_state->specific_shop_item_selected = item->specific_type;
        game_state->purchased_item_cost = price;

        printf("selected shop item! \n");
        return price;
    }
    printf("not enough money to purchase item \n");
    return 0;
}

void shop_reset_game_state(game_state_t *game_state, bool successful_purchase)
{
    if (successful_purchase)
    {
        game_state->money -= game_state->purchased_item_cost;
    }
    game_state->last_clicked_item_type = NOTHING;
    game_state->specific_shop_item_selected = 0;
    game_state->purchased_item_cost = 0;
    game_state->last_tower_selected = NULL;

    shop_undisplay_description(game_state);
    undisplay_help(game_state);
}

void shop_purchase_tower_upgrade(body_t *tower_body, game_state_t *game_state)
{
    // 1. player clicks on tower -- mouse_handler in CTD
    // 2. display upgrade button for selected tower -- shop_display_upgrade_button()
    // 3. if button is clicked, apply upgrade to selected tower -- shop_purchase_tower_upgrade

    tower_t *tower = get_global_secondary_info(tower_body);
    int tower_id = tower->id; // specific tower id

    if (tower->upgrade_level == UPGRADE_LEVEL_1)
    {
        tower_t *upgrade = malloc(sizeof(tower_t));
        switch (tower_id)
        {
        case DART_TOWER_ID:
            *upgrade = DART_TOWER_2;
            break;
        case TACK_SHOOTER_ID:
            *upgrade = TACK_SHOOTER_2;
            break;
        case BOMB_SHOOTER_ID:
            *upgrade = BOMB_SHOOTER_2;
            break;
        case SUPER_TOWER_ID:
            *upgrade = SUPER_TOWER_2;
            break;
        case BANK_ID:
            *upgrade = BANK_2;
            break;
        case AIRPLANE_ID:
            *upgrade = AIRPLANE_2;
            upgrade->flight_center = tower->flight_center;
            break;
        case GLUE_TOWER_ID:
            *upgrade = GLUE_TOWER_2;
            break;
        case BOOMERANG_ID:
            *upgrade = BOOMERANG_2;
            break;
        }
        if (game_state->money - upgrade->price >= 0)
        {
            game_state->money -= upgrade->price;
            body_set_info(tower_body, create_global_body_info(upgrade, TOWER_TYPE));
            body_set_color(tower_body, color_shade(body_get_color(tower_body)));
        }
    }
    else if (tower->upgrade_level == UPGRADE_LEVEL_2)
    {
        tower_t *upgrade = malloc(sizeof(tower_t));
        switch (tower_id)
        {
        case DART_TOWER_ID:
            *upgrade = DART_TOWER_3;
            break;
        case TACK_SHOOTER_ID:
            *upgrade = TACK_SHOOTER_3;
            break;
        case BOMB_SHOOTER_ID:
            *upgrade = BOMB_SHOOTER_3;
            break;
        case SUPER_TOWER_ID:
            *upgrade = SUPER_TOWER_3;
            break;
        case BANK_ID:
            *upgrade = BANK_3;
            break;
        case AIRPLANE_ID:
            *upgrade = AIRPLANE_3;
            upgrade->flight_center = tower->flight_center;
            break;
        case GLUE_TOWER_ID:
            *upgrade = GLUE_TOWER_3;
            break;
        case BOOMERANG_ID:
            *upgrade = BOOMERANG_3;
            break;
        }
        if (game_state->money - upgrade->price >= 0)
        {
            game_state->money -= upgrade->price;
            body_set_info(tower_body, create_global_body_info(upgrade, TOWER_TYPE));
            body_set_color(tower_body, color_shade(body_get_color(tower_body)));
        }
    }
    else if (tower->upgrade_level == UPGRADE_LEVEL_3)
    {
        tower_t *upgrade = malloc(sizeof(tower_t));
        switch (tower_id)
        {
        case DART_TOWER_ID:
            *upgrade = DART_TOWER_4;
            break;
        case TACK_SHOOTER_ID:
            *upgrade = TACK_SHOOTER_4;
            break;
        case BOMB_SHOOTER_ID:
            *upgrade = BOMB_SHOOTER_4;
            break;
        case SUPER_TOWER_ID:
            *upgrade = SUPER_TOWER_4;
            break;
        case BANK_ID:
            *upgrade = BANK_4;
            break;
        case AIRPLANE_ID:
            *upgrade = AIRPLANE_4;
            upgrade->flight_center = tower->flight_center;
            break;
        case GLUE_TOWER_ID:
            *upgrade = GLUE_TOWER_4;
            break;
        case BOOMERANG_ID:
            *upgrade = BOOMERANG_4;
            break;
        }
        if (game_state->money - upgrade->price >= 0)
        {
            game_state->money -= upgrade->price;
            body_set_info(tower_body, create_global_body_info(upgrade, TOWER_TYPE));
            body_set_color(tower_body, color_shade(body_get_color(tower_body)));
        }
    }
    else
    {
        printf("this tower is already upgraded to the max, no more upgrades available \n");
    }
    shop_reset_game_state(game_state, true);
}

void shop_display_upgrade_button(body_t *tower_body, game_state_t *game_state)
{
    game_state->last_tower_selected = tower_body;
    game_state->last_clicked_item_type = TOWER_TYPE;
    scene_t *scene = game_state->scene;
    tower_t *tower_info = (tower_t *)get_global_secondary_info(tower_body);
    tower_id_t tower_id = tower_info->id;

    if (tower_info->upgrade_level < UPGRADE_LEVEL_4)
    {
        // Create next upgrade button when a tower is clicked
        vector_t corner = {.x = SHOP_START.x, .y = SHOP_START.y / 2};
        vector_t upgrade_text_size = (vector_t){.x = UPGRADE_BUTTON_WIDTH, .y = UPGRADE_BUTTON_HEIGHT};
        list_t *button_shape = polygon_make_rectangle(corner.x, corner.y, corner.x + UPGRADE_BUTTON_WIDTH,
                                                      corner.y + UPGRADE_BUTTON_HEIGHT);
        global_body_info_t *info = create_global_body_info(NULL, UPGRADE_BUTTON_TYPE);
        TTF_Font *font = create_font("fonts/futura.ttf", SHOP_BUTTON_FONT_SIZE);
        char *label = malloc(sizeof(char) * 50);
        *label = '\0';
        strcat(label, "UPGRADE TOWER ");
        char lvl1[5];
        sprintf(lvl1, "%d", tower_info->upgrade_level);
        strcat(label, lvl1);
        strcat(label, "->");
        char lvl2[5];
        sprintf(lvl2, "%d", tower_info->upgrade_level + 1);
        strcat(label, lvl2);
        strcat(label, ": ");
        strcat(label, shop_price_label(tower_info->price));

        printf("%s \n", label);

        body_t *button = body_init_with_info_with_label(button_shape, ITEM_MASS, tower_get_color(tower_id),
                                                        info, NULL, label, SHOP_TEXT_COLOR, font, upgrade_text_size);
        free(label); // free because text_init mallocs again
        scene_add_body(scene, button);

        shop_display_description(game_state, tower_body);
    }
    else
    {
        printf("this tower is already upgraded to the max, no more upgrades available \n");
        shop_reset_game_state(game_state, false);
    }
}

void shop_display_description(game_state_t *game_state, body_t *body)
{
    scene_t *scene = game_state->scene;
    int global_type = get_global_type(body);
    TTF_Font *font = create_font("fonts/futura.ttf", 100);
    text_t *description;

    if (global_type == TOWER_TYPE) // display upgrade description (next level tower's description)
    {
        tower_t *tower_info = (tower_t *)get_global_secondary_info(body);
        char *upgrade_text = (tower_get_from_id_and_level(tower_info->id, tower_info->upgrade_level + 1)).description;
        description = text_init(upgrade_text, DESCRIPTION_POSITION, DESCRIPTION_SIZE, SHOP_TEXT_COLOR, font);
    }
    else if (global_type == SHOP_TYPE)
    {
        shop_item_t *shop_info = (shop_item_t *)get_global_secondary_info(body);
        description = text_init(shop_info->description, DESCRIPTION_POSITION, DESCRIPTION_SIZE, SHOP_TEXT_COLOR, font);
    } else {
        return;
    }

    scene_add_text(scene, description);
    game_state->shop_description = description;
}

void shop_undisplay_description(game_state_t *game_state)
{
    if (game_state->shop_description != NULL)
    {
        text_remove(game_state->shop_description);
        game_state->shop_description = NULL;
    }
}

void tower_undisplay_upgrade_button(scene_t *scene)
{
    for (int i = 0; i < scene_bodies(scene); i++)
    {
        body_t *body = scene_get_body(scene, i);
        if (get_global_type(body) == UPGRADE_BUTTON_TYPE)
        {
            scene_remove_body(scene, i);
        }
    }
}

bool is_within_window(vector_t mouse_pos, double demo_playing_width, double demo_playing_height, double radius)
{
    return (mouse_pos.x - radius > 0 && mouse_pos.x + radius < demo_playing_width && mouse_pos.y - radius > 0 && mouse_pos.y + radius < demo_playing_height);
}

// checks if mouse_pos (vector_t) is within the area of the path
// if so, adjusts mouse position and returns new position
bool is_within_path(body_t *path_body, vector_t mouse_pos, scene_t *scene, double radius) // private
{
    list_t *vertices = body_get_shape(path_body);
    size_t size = list_size(vertices);

    // for each rectangle
    for (size_t k = 0; k < 0.5 * size - 1; k++)
    {
        // getting opposite corners of rectangle
        vector_t outer = *((vector_t *)list_get(vertices, k));
        vector_t inner = *((vector_t *)list_get(vertices, size - (k + 2)));

        // checks the opposite of the needed condition
        double min_x = fmin(outer.x, inner.x) - radius;
        double min_y = fmin(outer.y, inner.y) - radius;
        double max_x = fmax(outer.x, inner.x) + radius;
        double max_y = fmax(outer.y, inner.y) + radius;

        // checking if the mouse position is within the rectangle
        // and checking if the mouse position is within the shop_menu
        if ((min_x <= mouse_pos.x && min_y <= mouse_pos.y && max_x >= mouse_pos.x && max_y >= mouse_pos.y)) 
        {
            return true;
        }
    }

    return false;
}

bool overlap_with_towers(vector_t mouse_pos, scene_t *scene, double radius)
{
    radius *= 2;
    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        body_t *b = scene_get_body(scene, i);
        if (get_global_type(b) == TOWER_TYPE)
        {
            if (vec_distance(body_get_centroid(b), mouse_pos) < radius)
            {
                return true;
            }
        }
    }
    return false;
}

void shop_place_item(game_state_t *game_state, vector_t mouse_pos, double demo_playing_width, double demo_playing_height)
{
    global_body_type_t item_id = game_state->global_shop_item_selected;
    bool successful_purchase = false;

    for (size_t i = 0; i < scene_bodies(game_state->scene); i++)
    {
        body_t *path_body = scene_get_body(game_state->scene, i);
        if (get_global_type(path_body) == PATH_TYPE)
        {
            // create tower at mouse position IF mouse_pos is outside path
            scene_t *scene = game_state->scene;
            if (item_id == TOWER_TYPE && !is_within_path(path_body, mouse_pos, scene, tower_get_radius()) && is_within_window(mouse_pos, demo_playing_width, demo_playing_height, tower_get_radius()) && !overlap_with_towers(mouse_pos, scene, tower_get_radius()))
            {
                int id = game_state->specific_shop_item_selected;
                body_t *b = create_tower(scene, tower_get_from_id(id), mouse_pos);
                if (id == AIRPLANE_ID)
                {
                    body_set_centroid(b, (vector_t){.x = mouse_pos.x + tower_get_airplane_flight_radius(), .y = mouse_pos.y});
                    tower_t *info = get_global_secondary_info(b);
                    info->flight_center = mouse_pos;
                }
                successful_purchase = true;
                shop_reset_game_state(game_state, successful_purchase);
                printf("purchased and placed a tower! \n");
                sound_play(game_state->purchase_sound);
                break;
            }
            else if (item_id == TOOL_TYPE && is_within_path(path_body, mouse_pos, game_state->scene, -0.5 * tool_get_radius()) && is_within_window(mouse_pos, demo_playing_width, demo_playing_height, tool_get_radius()))
            {
                // placing tools on the path
                int id = game_state->specific_shop_item_selected;
                create_tool(scene, tool_get_from_id(id), mouse_pos);
                successful_purchase = true;
                shop_reset_game_state(game_state, successful_purchase);
                printf("purchased and placed a tool! \n");
                sound_play(game_state->purchase_sound);
                break;
            }
            shop_undisplay_description(game_state);
        }
    }

    if (!successful_purchase)
    {
        shop_reset_game_state(game_state, successful_purchase);
        printf("invalid location, purchase canceled \n");
    }
}

void shop_temp_display(game_state_t *game_state, vector_t mouse_pos, double demo_playing_width, double demo_playing_height)
{
    int type = game_state->global_shop_item_selected;
    body_t *b = NULL;
    body_t *path_body = NULL;
    scene_t *scene = game_state->scene;

    for (size_t i = 0; i < scene_bodies(scene); i++)
    {
        body_t *curr_body = scene_get_body(scene, i);
        if (get_global_type(curr_body) == MOUSE_TYPE)
        {
            b = curr_body;
        }
        else if (get_global_type(curr_body) == PATH_TYPE)
        {
            path_body = curr_body;
        }
        if (b != NULL && path_body != NULL)
        {
            break;
        }
    }

    if (b == NULL)
    {
        list_t *shape = NULL;
        if (type == TOWER_TYPE)
        {
            shape = polygon_make_circle(mouse_pos, tower_get_radius(), SHOP_TOWER_CIRCLE_POINTS);
        }
        else if (type == TOOL_TYPE)
        {
            shape = polygon_make_square(mouse_pos, tool_get_radius());
        }
        b = body_init_with_secondary_info(shape, INFINITY, PASTEL_RED, NULL, NULL, MOUSE_TYPE);
        scene_add_body(game_state->scene, b);
    }

    body_set_centroid(b, mouse_pos);
    if (path_body != NULL)
    {
        body_set_color(b, PASTEL_RED);
        if (type == TOWER_TYPE)
        {
            if (is_within_window(mouse_pos, demo_playing_width, demo_playing_height, tower_get_radius()) && !is_within_path(path_body, mouse_pos, scene, tower_get_radius()) && !overlap_with_towers(mouse_pos, scene, tower_get_radius()))
            {
                body_set_color(b, PASTEL_GREEN);
            }
        }
        else if (type == TOOL_TYPE)
        {
            if (is_within_window(mouse_pos, demo_playing_width, demo_playing_height, tool_get_radius()) && is_within_path(path_body, mouse_pos, scene, -0.5 * tool_get_radius()))
            {
                body_set_color(b, PASTEL_GREEN);
            }
        }
    }
}

void display_help(game_state_t *game_state)
{
    image_t *help_image = image_init("images/help.png", HELP_POSITION,HELP_SIZE, IMG_INIT_PNG);
    game_state->help_image = help_image;
    scene_add_image(game_state->scene, help_image);
}

void undisplay_help(game_state_t *game_state)
{
    if (game_state->help_image)
    {
        image_remove(game_state->help_image);
        game_state->help_image = NULL;
    }
}
