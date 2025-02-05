// 403106624
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <locale.h>
#include <stdlib.h>
#define SQUARE(x) ((x) * (x))
#define MAX_NUM_USERS 50
//////////////////////////////////

typedef struct user
{
    char name[30];
    int score;
    int gold;
    int num_games;
    int experience;
} user;

typedef struct general_setting
{
    char USERNAME[31];
    int LEVEL;
    int COLOR;
    int num_enemies;
    int num_rooms;
} general_setting;

typedef struct point
{
    int x;
    int y;
} point;

typedef struct room
{
    struct point start;
    int room_type;
    int num_enemies;
    // 0 -->usual;
    // 1 -->enchant
    // 2 -->treasure
    int width;
    int height;
    int num_doors;
    struct point *doors;
    struct point *pillar;
    struct point traps;

} room;

typedef struct saving_room
{
    struct point start;
    int room_type;
    int num_enemies;
    // 0 -->usual;
    // 1 -->enchant
    int width;
    int height;
    int num_doors;

} saving_room;

typedef struct map
{
    int floor;
    int num_enemies;
    int num_rooms;
    struct point stairs;
    struct point master_key;
    struct room *rooms;
    char signed_map[92][32];
} map;

typedef struct saving_map
{
    int floor;
    int num_enemies;
    int num_rooms;
    struct point stairs;
    struct point master_key;
    struct saving_room rooms[7];
    char signed_map[92][32];
} saving_map;

typedef struct player
{
    struct point position;
    int initial_health;
    int health;
    int number_of_dagger;
    int number_of_mace;
    int number_of_wand;
    int number_of_arrow;
    int number_of_sword;
    int default_ammo;
    /*
    1 --> mace
    2 --> arrow
    3 --> sword
    4 --> wnad
    5 --> dagger
    0 --> nothing
    */
    int last_shot;
    /*
    0 -->nothing
    ab--> (a-->ammo) (b-->direction)
    direction:
    1 -->up
    2 -->up and right
    3 --> right
    4 --> right and down
    5 --> down
    6 --> down and left
    7 --> left
    8 -->left and up
    */
    int number_of_foods;
    int number_of_health_enchant;
    int number_of_speed_enchant;
    int number_of_damage_enchant;
    int usual_golds;
    int black_golds;
    /*
    1 --> red
    2 --> blue
    3 --> yellow
    4 --> green
    */
    char username[31];
    int number_of_master_keys;
    int number_of_broken_master_keys;
    int visited[92][32];
    int num_wrong_password;
    int showed_password;
    int password;
    int has_password;
    //
    int showed_room[7];
    //
    int initial_hunger;
    int hunger;
    int is_health_enchant_active;
    int is_damage_enchant_active;
    int is_speed_enchant_active;
    //
    int health_enchant_counter;
    int speed_enchant_counter;
    int damage_enchant_counter;
    //

    /*
    1-->Yes
    0-->No
    */
    time_t last_password;
    time_t last_time;
    time_t last_attack;
} player;

typedef struct enemy
{
    int type;
    /*
    1 --> Deamon //D
    2 --> Fire breathing monster //F
    3 --> Giant //G
    4 --> Snake //S
    5 --> undeed //U
    */
    int health;
    int initial_health;
    int available_move;
    int num_room;
    int move_counter;
    /*
    all except snake
    */
    struct point position;

} enemy;

typedef struct users_scoreboard
{
    int num_users;
    time_t first[MAX_NUM_USERS];
    char usernames[MAX_NUM_USERS][30];
    int score[MAX_NUM_USERS];
    int golds[MAX_NUM_USERS];
    int num_played_games[MAX_NUM_USERS];
    int experience[MAX_NUM_USERS];
} users_scoreboard;

general_setting GENERAL_SETTING;
player main_player;
map main_map;
enemy *enemies;
users_scoreboard main_scoreboard;

//////////////////////////////////

int is_available(char *username);
int valid_password(char *password);
int valid_email(char *email);
int create_user();
int login();
int game_menu();
int setting();
int scoreboard();
int main_menu();
void initialize_general_setting();
int make_new_game();
int is_overlapping(room r1, room r2);
void initialize_map();
void draw_in_map(room r);
int is_overlapping(room r1, room r2);
void draw_in_map(room r);
int generate_rooms(room *rooms, int num_rooms, int max_width, int max_height);
void add_doors(room *r);
void add_traps(room *r);
void add_pillar(room *r);
void add_stairs(room r);
int connectDoors(point doorone);
void add_corridor();
void add_master_key();
int have_hidden_door(room r);
void add_enchant_room(room *r);
void add_gold(room *r);
void add_enchant_usual(room *r);
void add_food(room *r);
void add_ammo(room *r);
void create_map();
void setup_player(player *p);
void initialize_random_position(player *p);
int inverse(int number);
int player_movement(int ch, int pick_up);
int recognize_rooms(point p);
void print_room(int i);
void print_game();
void next_floor();
int end_game();
int pause_menu();
int save_game();
void consume_food();
void search_around();
void view_whole_map();
void list_of_ammos();
void initialize_enemies();
void move_enemy();
int calculate_nearer(point p1, point p2, point p3, point p4);
void damage_to_enemy();
void damage_to_player();
void add_treasure();
void draw_treasure_room();
int resume_game();
int resume_last_game();
void print_wall();
void show_tombstone(int score);
void show_victory(int score);
int profile_menu();
////////////////////////////////////////////////////////////////////
int profile_menu()
{
    clear();
    if (strcmp(GENERAL_SETTING.USERNAME, "Guest") == 0)
    {
        attron(COLOR_PAIR(5));
        mvprintw(16, 35, "%s", "You must login first!");
        attroff(COLOR_PAIR(5));
        refresh();
        napms(2000);
        clear();
        return 0;
    }

    char *login_file_path = (char *)malloc(100 * sizeof(char));
    char *username = (char *)malloc(31 * sizeof(char));
    char *password = (char *)malloc(31 * sizeof(char));
    char *email = (char *)malloc(51 * sizeof(char));
    char *password_1 = (char *)malloc(31 * sizeof(char));
    char *password_2 = (char *)malloc(31 * sizeof(char));
    char *password_3 = (char *)malloc(31 * sizeof(char));

    sprintf(login_file_path, "%s/%s_login.txt", GENERAL_SETTING.USERNAME, GENERAL_SETTING.USERNAME);
    FILE *file = fopen(login_file_path, "r");

    fgets(username, 31, file);
    fgets(password, 31, file);
    fgets(email, 51, file);

    strcpy(password_1, password);
    strcpy(password_2, password);
    strcpy(password_3, password);

    int score = 1;

    if (strlen(password) >= 10 && strlen(password) < 15)
        ++score;
    if (strlen(password) >= 15)
        score += 2;

    const char *special_chars = "~`!@#$^&*()_-+=[]{}\\;:'\"<,>.?/";

    while (*password_1)
    {
        if (strchr(special_chars, *password_1))
        {
            ++score;
            break;
        }
        ++password_1;
    }

    if (!(strstr(password_2, username) || strstr(password_3, email)))
    {
        ++score;
    }

    mvprintw(1, 1, "Username: %s", username);
    mvprintw(3, 1, "Password: %s", password);
    for (int i = 0; i < score + 1; ++i)
    {
        if (score == 5 || score == 4)
        {
            if (i == score)
            {
                mvprintw(3, 11 + i + strlen(password), "%s", " (strength)");
                break;
            }
            attron(COLOR_PAIR(6));
            mvprintw(3, 11 + i + strlen(password), "%s", "\u25Ac");
            attroff(COLOR_PAIR(6));
        }
        else if (score == 1 || score == 2)
        {
            if (i == score)
            {
                mvprintw(3, 11 + i + strlen(password), "%s", " (strength)");
                break;
            }
            attron(COLOR_PAIR(5));
            mvprintw(3, 11 + i + strlen(password), "%s", "\u25Ac");
            attroff(COLOR_PAIR(5));
        }
        else
        {
            if (i == score)
            {
                mvprintw(3, 11 + i + strlen(password), "%s", " (strength)");
                break;
            }
            attron(COLOR_PAIR(8));
            mvprintw(3, 11 + i + strlen(password), "%s", "\u25Ac");
            attroff(COLOR_PAIR(8));
        }
    }
    mvprintw(5, 1, "Email address: %s", email);
    mvprintw(31, 30, "%s", "Please press Enter to return to menu");
    refresh();

    noecho();
    keypad(stdscr, TRUE);

    int ch;
    ch = getch();
    while (ch != 10)
    {
        ch = getch();
    }

    clear();

    free(username);
    free(password);
    free(password_1);
    free(password_2);
    free(password_3);
    free(email);
    free(login_file_path);
    return 0;
}
void show_tombstone(int score)
{

    attron(COLOR_PAIR(8));
    mvprintw(10, 35, "+--------------------+");
    for (int i = 11; i <= 20; ++i)
    {
        mvprintw(i, 35, "|                    |");
    }
    mvprintw(21, 35, "+--------------------+");
    attroff(COLOR_PAIR(8));

    attron(COLOR_PAIR(5));
    mvprintw(13, 42, "%s", "GAME OVER");
    attroff(COLOR_PAIR(5));

    mvprintw(16, 40, "%s", "Your score: ");
    attron(COLOR_PAIR(6));
    printw("%d", score);
    attroff(COLOR_PAIR(6));

    refresh();
    napms(2000);
}

void show_victory(int score)
{

    attron(COLOR_PAIR(8));
    mvprintw(10, 35, "+--------------------+");
    for (int i = 11; i <= 20; ++i)
    {
        mvprintw(i, 35, "|                    |");
    }
    mvprintw(21, 35, "+--------------------+");
    attroff(COLOR_PAIR(8));

    attron(COLOR_PAIR(7));
    mvprintw(13, 42, "%s", "VICTORY");
    attroff(COLOR_PAIR(7));

    mvprintw(16, 40, "%s", "Your score: ");
    attron(COLOR_PAIR(6));
    printw("%d", score);
    attroff(COLOR_PAIR(6));

    refresh();
    napms(2000);
}

int resume_game()
{
    char *map_save = (char *)malloc(50 * sizeof(char));
    char *player_save = (char *)malloc(50 * sizeof(char));
    char *general_setting_save = (char *)malloc(50 * sizeof(char));
    char *enemies_save = (char *)malloc(50 * sizeof(char));

    sprintf(map_save, "%s/map.bin", GENERAL_SETTING.USERNAME);
    sprintf(player_save, "%s/player.bin", GENERAL_SETTING.USERNAME);
    sprintf(general_setting_save, "%s/generalsetting.bin", GENERAL_SETTING.USERNAME);
    sprintf(enemies_save, "%s/enemies.bin", GENERAL_SETTING.USERNAME);

    FILE *file_1 = fopen(general_setting_save, "rb");
    fread(&GENERAL_SETTING, sizeof(general_setting), 1, file_1);
    fclose(file_1);

    FILE *file_2 = fopen(player_save, "rb");
    fread(&main_player, sizeof(player), 1, file_2);
    fclose(file_2);

    saving_map copy_map;
    FILE *file_3 = fopen(map_save, "rb");
    fread(&copy_map, sizeof(saving_map), 1, file_3);
    fclose(file_3);

    enemy saving_enemies[GENERAL_SETTING.num_enemies];
    FILE *file_4 = fopen(enemies_save, "rb");
    fread(&saving_enemies, sizeof(enemy), GENERAL_SETTING.num_enemies, file_4);
    fclose(file_4);

    enemies = (enemy *)malloc(sizeof(enemy) * GENERAL_SETTING.num_enemies);
    for (int i = 0; i < GENERAL_SETTING.num_enemies; ++i)
    {
        enemies[i] = saving_enemies[i];
    }

    main_map.floor = copy_map.floor;
    main_map.master_key.x = copy_map.master_key.x;
    main_map.master_key.y = copy_map.master_key.y;
    main_map.num_enemies = copy_map.num_enemies;
    main_map.num_rooms = copy_map.num_rooms;
    main_map.stairs.x = copy_map.stairs.x;
    main_map.stairs.y = copy_map.stairs.y;

    for (int i = 0; i < 92; ++i)
    {
        for (int j = 0; j < 32; ++j)
        {
            main_map.signed_map[i][j] = copy_map.signed_map[i][j];
        }
    }

    main_map.rooms = (room *)malloc(sizeof(room) * GENERAL_SETTING.num_rooms);
    for (int i = 0; i < GENERAL_SETTING.num_rooms; ++i)
    {
        main_map.rooms[i].start.x = copy_map.rooms[i].start.x;
        main_map.rooms[i].start.y = copy_map.rooms[i].start.y;
        main_map.rooms[i].width = copy_map.rooms[i].width;
        main_map.rooms[i].height = copy_map.rooms[i].height;
        main_map.rooms[i].num_doors = copy_map.rooms[i].num_doors;
        main_map.rooms[i].num_enemies = copy_map.rooms[i].num_enemies;
        main_map.rooms[i].room_type = copy_map.rooms[i].room_type;
    }

    free(general_setting_save);
    free(map_save);
    free(player_save);
    free(enemies_save);

    return resume_last_game();
}
void draw_treasure_room()
{

    clear();
    attron(COLOR_PAIR(1));
    mvprintw(14, 35, "%s", "You're moved to treasure room");
    attroff(COLOR_PAIR(1));
    refresh();

    enemies = NULL;
    main_map.floor = 5;
    main_map.num_rooms = 1;
    main_map.rooms = (room *)malloc(sizeof(room));
    initialize_map();
    for (int i = 0; i < 92; ++i)
    {
        for (int j = 0; j < 32; ++j)
        {
            main_player.visited[i][j] = 0;
        }
    }

    main_map.rooms[0].doors = NULL;
    main_map.rooms[0].height = (rand() % 6) + 15;
    main_map.rooms[0].num_doors = 0;
    main_map.rooms[0].num_enemies = 0;
    main_map.rooms[0].pillar = NULL;
    main_map.rooms[0].room_type = 2;
    main_map.rooms[0].start.x = (rand() % 5) + 20;
    main_map.rooms[0].start.y = (rand() % 2) + 5;
    main_map.rooms[0].width = (rand() % 11) + 25;

    draw_in_map(main_map.rooms[0]);
    add_pillar(&main_map.rooms[0]);

    int area = main_map.rooms[0].height * main_map.rooms[0].width;

    while (area > 0)
    {
        int random_x = (rand() % (main_map.rooms[0].width - 1)) + main_map.rooms[0].start.x + 1;
        int random_y = (rand() % (main_map.rooms[0].height - 1)) + main_map.rooms[0].start.y + 1;

        area -= 20;
        if (main_map.signed_map[random_x][random_y] != '.')
            continue;
        area -= 20;
        main_map.signed_map[random_x][random_y] = 't';
    }

    add_gold(&main_map.rooms[0]);
    add_enchant_room(&main_map.rooms[0]);
    add_ammo(&main_map.rooms[0]);
    initialize_enemies();
    initialize_random_position(&main_player);
    if (strcmp(GENERAL_SETTING.USERNAME, "Guest") != 0)
    {
        save_game();
    }
    napms(700);
    clear();
    print_game();
}

int is_overlapping(room r1, room r2)
{
    return !(r1.start.x + r1.width + 5 < r2.start.x ||
             r2.start.x + r2.width + 5 < r1.start.x ||
             r1.start.y + r1.height + 3 < r2.start.y ||
             r2.start.y + r2.height + 3 < r1.start.y);
}

void initialize_map()
{
    main_map.num_enemies = 0;
    for (int x = 0; x < 92; x++)
    {
        for (int y = 0; y < 30; ++y)
        {
            main_map.signed_map[x][y] = ' ';
        }
    }
}

void initialize_enemies()
{
    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        if (main_map.rooms[i].room_type == 1)
            continue;
        int area = main_map.rooms[i].width * main_map.rooms[i].height;
        if (GENERAL_SETTING.LEVEL == 2)
            area += 40;
        if (GENERAL_SETTING.LEVEL == 3)
            area += 60;

        if (main_map.floor == 5)
        {
            main_map.rooms[i].num_enemies = area / 60;
        }
        else if (main_map.floor >= 3 || area > 40)
        {
            main_map.rooms[i].num_enemies = (rand() % 2) + 1;
        }
        else
        {
            main_map.rooms[i].num_enemies = (rand() % 2);
        }

        main_map.num_enemies += main_map.rooms[i].num_enemies;
        enemies = (enemy *)realloc(enemies, sizeof(enemy) * main_map.num_enemies);
        for (int j = 0; j < main_map.rooms[i].num_enemies; ++j)
        {
            if (main_map.floor == 5)
            {
                if (rand() % 3 == 0)
                {
                    enemies[main_map.num_enemies - j - 1].type = 4;
                }
                else
                {
                    enemies[main_map.num_enemies - j - 1].type = rand() % 5 + 1;
                }
            }
            else
            {
                enemies[main_map.num_enemies - j - 1].type = rand() % 5 + 1;
            }

            switch (enemies[main_map.num_enemies - j - 1].type)
            {
            case 1:
            {
                enemies[main_map.num_enemies - j - 1].available_move = 1;
                enemies[main_map.num_enemies - j - 1].num_room = i;
                enemies[main_map.num_enemies - j - 1].health = 5;
                enemies[main_map.num_enemies - j - 1].initial_health = 5;
                enemies[main_map.num_enemies - j - 1].move_counter = 15;
            }
            break;

            case 2:
            {
                enemies[main_map.num_enemies - j - 1].available_move = 1;
                enemies[main_map.num_enemies - j - 1].num_room = i;
                enemies[main_map.num_enemies - j - 1].health = 10;
                enemies[main_map.num_enemies - j - 1].initial_health = 10;
                enemies[main_map.num_enemies - j - 1].move_counter = 15;
            }
            break;

            case 3:
            {
                enemies[main_map.num_enemies - j - 1].available_move = 1;
                enemies[main_map.num_enemies - j - 1].num_room = i;
                enemies[main_map.num_enemies - j - 1].health = 15;
                enemies[main_map.num_enemies - j - 1].initial_health = 15;
                enemies[main_map.num_enemies - j - 1].move_counter = 0;
            }
            break;

            case 4:
            {
                enemies[main_map.num_enemies - j - 1].available_move = 1;
                enemies[main_map.num_enemies - j - 1].num_room = i;
                enemies[main_map.num_enemies - j - 1].health = 20;
                enemies[main_map.num_enemies - j - 1].initial_health = 20;
                enemies[main_map.num_enemies - j - 1].move_counter = 10000;
            }
            break;

            case 5:
            {
                enemies[main_map.num_enemies - j - 1].available_move = 1;
                enemies[main_map.num_enemies - j - 1].num_room = i;
                enemies[main_map.num_enemies - j - 1].health = 30;
                enemies[main_map.num_enemies - j - 1].initial_health = 30;
                enemies[main_map.num_enemies - j - 1].move_counter = 0;
            }
            break;

            default:
                break;
            }

            int counter = 0;
            while (1)
            {
                if (counter == 20)
                    break;
                ++counter;
                int x = (rand() % (main_map.rooms[i].width - 1)) + main_map.rooms[i].start.x + 1;
                int y = (rand() % (main_map.rooms[i].height - 1)) + main_map.rooms[i].start.y + 1;

                if (main_map.signed_map[x][y] == '.')
                {

                    enemies[main_map.num_enemies - j - 1].position.x = x;
                    enemies[main_map.num_enemies - j - 1].position.y = y;
                    break;
                }
            }
        }
    }
}

void draw_in_map(room r)
{

    for (int x = r.start.x; x < r.start.x + r.width + 1; x++)
    {
        main_map.signed_map[x][r.start.y] = '-';
        main_map.signed_map[x][r.start.y + r.height] = '-';
    }

    // r.start.x<=x<=r.start.x+r.width,y=r.start.y
    // r.start.x<=x<r.start.x+r.width,y=r.start.y+r.height

    for (int y = r.start.y + 1; y < r.start.y + r.height; y++)
    {
        main_map.signed_map[r.start.x][y] = '|';
        main_map.signed_map[r.start.x + r.width][y] = '|';
    }
    // r.start.y+1<=y<r.start.y+r.height, x=r.start.x
    // r.start.y+1<=y<r.start.y+r.height, x=r.start.x + r.width

    for (int y = r.start.y + 1; y < r.start.y + r.height; y++)
    {
        for (int x = r.start.x + 1; x < r.start.x + r.width; x++)
        {
            main_map.signed_map[x][y] = '.';
        }
    }
}

int generate_rooms(room *rooms, int num_rooms, int max_width, int max_height)
{
    int counter = 0;
    for (int i = 0; i < num_rooms; ++i)
    {
        int valid = 0;
        while (!valid)
        {
            if (counter == 100)
                return 100;
            ++counter;
            room temp;
            temp.width = (rand() % (max_width - 4) + 1) + 4;   // 5<=x    &&     x+width<=86-->x<=86-width;
            temp.height = (rand() % (max_height - 4) + 1) + 4; // 3<=y    &&     y+width<=28-->y<=28-width;
            temp.start.x = (rand() % (84 - temp.width) + 1) + 2;
            temp.start.y = (rand() % (23 - temp.height) + 1) + 2;

            valid = 1;
            for (int j = 0; j < i; ++j)
            {
                if (is_overlapping(temp, rooms[j]))
                {
                    valid = 0;
                    break;
                }
            }
            if (valid)
            {
                rooms[i] = temp;
                rooms[i].num_enemies = 0;
            }
        }
    }
}

void add_doors(room *r)
{
    int pass_doors = 0;
    int room_corners_x[2] = {r->start.x + 1, r->start.x + r->width - 1};
    int room_corners_y[2] = {r->start.y + 1, r->start.y + r->height - 1};
    r->num_doors = 0;
    r->doors = NULL;

    // r.start.x<=x<=r.start.x+r.width,y=r.start.y
    // r.start.x<=x<r.start.x+r.width,y=r.start.y+r.height
    if (rand() % 10 != 0)
    {
        if (r->start.y < 16)
        {
            ++r->num_doors;
            r->doors = (point *)realloc(r->doors, sizeof(point) * (r->num_doors));

            r->doors[r->num_doors - 1].x = r->start.x + (rand() % (r->width - 4)) + 2;
            r->doors[r->num_doors - 1].y = r->start.y;

            if ((rand() % 5 == 0) && (pass_doors == 0))
            {
                point password_position;
                while (1)
                {
                    password_position.x = room_corners_x[rand() % 2];
                    password_position.y = room_corners_y[rand() % 2];

                    if (main_map.signed_map[password_position.x][password_position.y] != '.')
                        continue;
                    main_map.signed_map[password_position.x][password_position.y] = '&';
                    ++pass_doors;
                    break;
                }

                main_map.signed_map[r->doors[r->num_doors - 1].x][r->doors[r->num_doors - 1].y] = 'R';
            }
            else
            {
                main_map.signed_map[r->doors[r->num_doors - 1].x][r->doors[r->num_doors - 1].y] = '+';
            }
        }
    }

    if (rand() % 10 != 0)
    {
        if (r->start.y > 16)
        {
            ++r->num_doors;
            r->doors = (point *)realloc(r->doors, sizeof(point) * (r->num_doors));

            r->doors[r->num_doors - 1].x = r->start.x + (rand() % (r->width - 4)) + 2;
            r->doors[r->num_doors - 1].y = r->start.y + r->height;

            if ((rand() % 5 == 0) && (pass_doors == 0))
            {
                point password_position;
                while (1)
                {
                    password_position.x = room_corners_x[rand() % 2];
                    password_position.y = room_corners_y[rand() % 2];

                    if (main_map.signed_map[password_position.x][password_position.y] != '.')
                        continue;
                    main_map.signed_map[password_position.x][password_position.y] = '&';
                    ++pass_doors;
                    break;
                }

                main_map.signed_map[r->doors[r->num_doors - 1].x][r->doors[r->num_doors - 1].y] = 'R';
            }
            else
            {
                main_map.signed_map[r->doors[r->num_doors - 1].x][r->doors[r->num_doors - 1].y] = '+';
            }
        }
    }

    // r.start.y+1<=y<r.start.y+r.height, x=r.start.x
    // r.start.y+1<=y<r.start.y+r.height, x=r.start.x + r.width
    if (rand() % 10 != 0)
    {
        if (r->start.x < 45)
        {
            ++r->num_doors;
            r->doors = (point *)realloc(r->doors, sizeof(point) * (r->num_doors));

            r->doors[r->num_doors - 1].x = r->start.x;
            r->doors[r->num_doors - 1].y = r->start.y + (rand() % (r->height - 4)) + 3;

            if ((rand() % 5 == 0) && (pass_doors == 0))
            {
                point password_position;
                while (1)
                {
                    password_position.x = room_corners_x[rand() % 2];
                    password_position.y = room_corners_y[rand() % 2];

                    if (main_map.signed_map[password_position.x][password_position.y] != '.')
                        continue;
                    main_map.signed_map[password_position.x][password_position.y] = '&';
                    ++pass_doors;
                    break;
                }

                main_map.signed_map[r->doors[r->num_doors - 1].x][r->doors[r->num_doors - 1].y] = 'R';
            }
            else
            {
                main_map.signed_map[r->doors[r->num_doors - 1].x][r->doors[r->num_doors - 1].y] = '+';
            }
        }
    }

    if (rand() % 10 != 0)
    {
        if (r->start.x > 45)
        {

            ++r->num_doors;
            r->doors = (point *)realloc(r->doors, sizeof(point) * (r->num_doors));

            r->doors[r->num_doors - 1].x = r->start.x + r->width;
            r->doors[r->num_doors - 1].y = r->start.y + (rand() % (r->height - 4)) + 3;

            if ((rand() % 5 == 0) && (pass_doors == 0))
            {
                point password_position;
                while (1)
                {
                    password_position.x = room_corners_x[rand() % 2];
                    password_position.y = room_corners_y[rand() % 2];

                    if (main_map.signed_map[password_position.x][password_position.y] != '.')
                        continue;
                    main_map.signed_map[password_position.x][password_position.y] = '&';
                    ++pass_doors;
                    break;
                }

                main_map.signed_map[r->doors[r->num_doors - 1].x][r->doors[r->num_doors - 1].y] = 'R';
            }
            else
            {
                main_map.signed_map[r->doors[r->num_doors - 1].x][r->doors[r->num_doors - 1].y] = '+';
            }
        }
    }

    if (r->num_doors == 0)
        add_doors(r);

    if ((r->num_doors == 1))
    {
        if (main_map.signed_map[r->doors[0].x][r->doors[0].y] == 'R')
        {
            for (int a = r->start.x; a <= r->start.x + r->width; ++a)
            {
                for (int b = r->start.y; b <= r->start.y + r->height; ++b)
                {
                    if (main_map.signed_map[a][b] == '&')
                        main_map.signed_map[a][b] = '.';
                }
            }
        }

        main_map.signed_map[r->doors[0].x][r->doors[0].y] = 'h';
    }

    if ((r->num_doors == 1))
    {
        if (main_map.signed_map[r->doors[0].x][r->doors[0].y] == 'R')
            add_doors(r);
    }
}

void add_traps(room *r)
{
    if ((rand() % 2) == 0)
    {
        r->traps.x = (rand() % (r->width - 2)) + r->start.x + 2;
        r->traps.y = (rand() % (r->height - 2)) + r->start.y + 2;
        main_map.signed_map[r->traps.x][r->traps.y] = 't';
    }
}

void add_pillar(room *r)
{
    srand(time(NULL));

    int area = (r->width) * (r->height);
    int counter = 0;

    r->pillar = NULL;

    while (area > 0)
    {

        if ((rand() % 3) == 0)
        {
            r->pillar = (point *)realloc(r->pillar, sizeof(point) * (counter + 1));

            r->pillar[counter].x = (rand() % (r->width - 3)) + r->start.x + 2;
            r->pillar[counter].y = (rand() % (r->height - 3)) + r->start.y + 2;

            if (main_map.signed_map[r->pillar[counter].x][r->pillar[counter].y] == 'o' || (r->pillar[counter].x == r->traps.x && r->pillar->y == r->traps.y))
                area += 20;
            else
            {
                main_map.signed_map[r->pillar[counter].x][r->pillar[counter].y] = 'o';
                ++counter;
            }
        }
        area -= 20;
    }
}

void add_stairs(room r)
{
    if (main_map.floor == 4)
        return;
    int i = (rand() % (r.width - 1)) + r.start.x + 1;
    int j = (rand() % (r.height - 1)) + r.start.y + 1;

    if (main_map.signed_map[i][j] == '.')
    {
        main_map.signed_map[i][j] = '<';
        main_map.stairs.x = i;
        main_map.stairs.y = j;
        return;
    }

    int random = (rand() % (main_map.num_rooms));
    add_stairs(main_map.rooms[random]);
}

int connectDoors(point doorone)
{
    int collision = 0;
    int counter = 0;
    point temp = doorone;
    int dx[4] = {1, 0, -1, 0};
    int dy[4] = {0, 1, 0, -1};

    int state = 0;
    point previous = temp;

    while (1)
    {

        point pre_previous = previous;
        counter++;
        for (int i = 0; i < 4; ++i)
        {
            if (temp.x + dx[i] == previous.x && temp.y + dy[i] == previous.y)
                continue;
            if (main_map.signed_map[temp.x + dx[i]][temp.y + dy[i]] == '#')
                return 0;
        }

        previous = temp;

        temp.x += dx[state];
        temp.y += dy[state];

        if (main_map.signed_map[temp.x][temp.y] != ' ' || temp.x < 0 || temp.x > 87 || temp.y < 1 || temp.y > 28)
        {
            previous = pre_previous;
            ++collision;
            temp.x -= dx[state];
            temp.y -= dy[state];
            if (collision % 3 == 0)
            {
                state = (state == 0) ? 3 : state - 1;
            }
            else
            {
                state = (state == 3) ? 0 : state + 1;
            }
        }
        else
        {
            main_map.signed_map[temp.x][temp.y] = '#';
        }
    }

    return 1;
}

void add_corridor()
{
    int num_total_doors = 0;
    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        num_total_doors += main_map.rooms[i].num_doors;
    }

    point *total_doors_array = (point *)malloc(sizeof(point) * num_total_doors);
    int counter = 0;
    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        for (int j = 0; j < main_map.rooms[i].num_doors; ++j)
        {
            total_doors_array[counter++] = main_map.rooms[i].doors[j];
        }
    }

    for (int i = 0; i < num_total_doors; ++i)
    {
        connectDoors(total_doors_array[i]);
    }
}

void add_master_key()
{
    int random_room = (rand() % (main_map.num_rooms));

    point random_position;

    while (1)
    {
        random_position.x = (rand() % (main_map.rooms[random_room].width - 1)) + main_map.rooms[random_room].start.x + 1;
        random_position.y = (rand() % (main_map.rooms[random_room].height - 1)) + main_map.rooms[random_room].start.y + 1;
        if (main_map.signed_map[random_position.x][random_position.y] != '.')
            continue;
        main_map.signed_map[random_position.x][random_position.y] = 'M';
        break;
    }
}

int have_hidden_door(room r)
{
    for (int i = r.start.x; i < r.start.x + r.width; ++i)
    {
        for (int j = r.start.y; j < r.start.y + r.height; ++j)
        {
            if (main_map.signed_map[i][j] == 'h')
                return 1;
        }
    }

    return 0;
}

void add_enchant_room(room *r)
{
    srand(time(NULL));
    point random_position;
    int area = r->width * r->height;
    if (main_map.floor == 5)
        area /= 4;

    while (area > 0)
    {
        random_position.x = (rand() % (r->width - 1)) + r->start.x + 1;
        random_position.y = (rand() % (r->height - 1)) + r->start.y + 1;
        area -= 8;
        if (main_map.signed_map[random_position.x][random_position.y] != '.')
            continue;
        int random = rand() % 4;
        if (random == 0 || random == 2)
        {
            main_map.signed_map[random_position.x][random_position.y] = 'H';
        }
        else if (random == 1)
        {
            main_map.signed_map[random_position.x][random_position.y] = 'S';
        }
        else
        {
            main_map.signed_map[random_position.x][random_position.y] = 'D';
        }
    }
}

void add_gold(room *r)
{
    srand(time(NULL));

    int area = r->height * r->width;
    point random_position;

    while (area > 0)
    {

        random_position.x = (rand() % (r->width - 1)) + r->start.x + 1;
        random_position.y = (rand() % (r->height - 1)) + r->start.y + 1;
        area -= 20;
        if (main_map.signed_map[random_position.x][random_position.y] != '.')
            continue;
        area -= 10;
        if (rand() % 5 == 1)
        {
            main_map.signed_map[random_position.x][random_position.y] = 'b';
        }
        else
        {
            main_map.signed_map[random_position.x][random_position.y] = 'g';
        }
    }
}

void add_enchant_usual(room *r)
{
    srand(time(NULL));
    int counter = 0;
    point random_position;

    while (1)
    {

        if (counter == 2)
            break;
        random_position.x = (rand() % (r->width - 1)) + r->start.x + 1;
        random_position.y = (rand() % (r->height - 1)) + r->start.y + 1;
        ++counter;
        if (main_map.signed_map[random_position.x][random_position.y] != '.')
            continue;
        int random = rand() % 3;
        if (random == 0)
        {
            main_map.signed_map[random_position.x][random_position.y] = 'H';
        }
        else if (random == 1)
        {
            main_map.signed_map[random_position.x][random_position.y] = 'S';
        }
        else
        {
            main_map.signed_map[random_position.x][random_position.y] = 'D';
        }
    }
}

void add_food(room *r)
{
    srand(time(NULL));
    point random_position;
    int area = r->width * r->height;

    while (area > 0)
    {

        random_position.x = (rand() % (r->width - 1)) + r->start.x + 1;
        random_position.y = (rand() % (r->height - 1)) + r->start.y + 1;
        area -= 12;
        if (main_map.signed_map[random_position.x][random_position.y] != '.')
            continue;
        main_map.signed_map[random_position.x][random_position.y] = 'f';
    }
}

void add_ammo(room *r)
{
    // d --> dagger
    // w --> wand
    // a --> arrow
    // s --> sword

    srand(time(NULL));
    point random_position;
    int area = r->width * r->height;
    if (main_map.floor == 5)
        area /= 2;

    while (area > 0)
    {

        random_position.x = (rand() % (r->width - 1)) + r->start.x + 1;
        random_position.y = (rand() % (r->height - 1)) + r->start.y + 1;
        area -= 10;
        if (main_map.signed_map[random_position.x][random_position.y] != '.')
            continue;

        int random = rand() % 4;

        if (random == 0)
        {
            main_map.signed_map[random_position.x][random_position.y] = 'd';
        }
        else if (random == 1)
        {
            main_map.signed_map[random_position.x][random_position.y] = 's';
        }
        else if (random == 2)
        {
            main_map.signed_map[random_position.x][random_position.y] = 'w';
        }
        else if (random == 3)
        {
            main_map.signed_map[random_position.x][random_position.y] = 'a';
        }
    }
}

void add_treasure(room r)
{
    int i = (rand() % (r.width - 1)) + r.start.x + 1;
    int j = (rand() % (r.height - 1)) + r.start.y + 1;

    if (main_map.signed_map[i][j] == '.')
    {
        main_map.signed_map[i][j] = 'T';
        main_map.stairs.x = i;
        main_map.stairs.y = j;
        return;
    }

    int random = (rand() % (main_map.num_rooms));
    add_stairs(main_map.rooms[random]);
}

void create_map()
{
    srand(time(0));

    initialize_map();

    int min_rooms = 6;
    int max_rooms = 7;

    main_map.num_rooms = (rand() % (max_rooms - min_rooms) + 1) + min_rooms;

    main_map.rooms = (struct room *)malloc(sizeof(room) * (main_map.num_rooms));

    if (generate_rooms(main_map.rooms, main_map.num_rooms, 9, 8) == 100)
        create_map();

    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        main_map.rooms[i].traps.x = -1;
        main_map.rooms[i].traps.y = -1;
        main_map.rooms[i].room_type = 0;
    }

    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        draw_in_map(main_map.rooms[i]);
    }

    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        add_doors(&(main_map.rooms[i]));
    }

    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        add_traps(&(main_map.rooms[i]));
    }

    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        add_pillar(&(main_map.rooms[i]));
    }

    if (main_map.floor != 4)
    {
        int random = (rand() % (main_map.num_rooms));
        add_stairs(main_map.rooms[random]);
    }

    if (main_map.floor == 4)
    {
        int random = (rand() % (main_map.num_rooms));
        add_treasure(main_map.rooms[random]);
    }

    add_corridor();
    add_master_key();

    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        if (have_hidden_door(main_map.rooms[i]) == 1)
        {
            main_map.rooms[i].room_type = 1;
            add_enchant_room(&(main_map.rooms[i]));
        }
    }

    for (int i = 0; i < main_map.num_rooms; ++i)
    {

        if (main_map.rooms[i].room_type == 0)
        {
            add_gold(&(main_map.rooms[i]));
            add_enchant_usual(&(main_map.rooms[i]));
            add_food(&(main_map.rooms[i]));
            add_ammo(&(main_map.rooms[i]));
        }
    }
}

int is_available(char *username)
{
    char *path = username;
    struct stat st;

    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
    {
        return 1;
    }

    if (strcmp(username, "Guest") == 0)
        return 1;

    return 0;
}

int valid_password(char *password)
{
    if (strlen(password) > 30 || strlen(password) < 7)
        return 0;

    int counter_digit = 0;
    for (int i = 0; i < strlen(password); ++i)
    {
        if ((password[i] > 47) && (password[i] < 58))
        {
            counter_digit += 1;
        }
    }

    if (counter_digit == 0)
        return 0;

    int counter_upper = 0;
    for (int i = 0; i < strlen(password); ++i)
    {
        if ((password[i] > 64) && (password[i] < 91))
        {
            counter_upper += 1;
        }
    }

    if (counter_upper == 0)
        return 0;

    int counter_lower = 0;
    for (int i = 0; i < strlen(password); ++i)
    {
        if ((password[i] > 96) && (password[i] < 122))
        {
            counter_lower += 1;
        }
    }

    if (counter_lower == 0)
        return 0;

    return 1;
}

int valid_email(char *email)
{
    int counter = 0;
    int at_place;
    int dot_place;
    for (int i = 0; i < strlen(email); ++i)
    {
        if (email[i] == '@')
        {
            ++counter;
            at_place = i;
        }
    }

    if (counter != 1)
        return 0;
    if (at_place == 0)
        return 0;
    counter = 0;

    for (int i = at_place; i < strlen(email); ++i)
    {
        if (email[i] == '.')
        {
            ++counter;
            dot_place = i;
        }
    }

    if (counter != 1)
        return 0;
    if (dot_place == at_place + 1 || dot_place == strlen(email - 1))
        return 0;
    return 1;
}

int create_user()
{
    clear();
    char *options[] = {"Create a user", "Exit"};
    int option_numbers = 2;
    int highlight = 0;
    int ch;
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    while (1)
    {
        for (int i = 0; i < 2; ++i)
        {
            if (highlight == i)
            {
                attron(A_REVERSE);
                attron(A_BOLD);
                mvprintw(2 * i + 1, 2, "%s", options[i]);
                attroff(A_BOLD);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(2 * i + 1, 2, "%s", options[i]);
            }
        }
        refresh();
        ch = getch();

        if (ch == KEY_DOWN)
        {
            if (highlight < 1)
                ++highlight;
        }
        else if (ch == KEY_UP)
        {
            if (highlight > 0)
                --highlight;
        }
        else if (ch == 10)
        {
            if (highlight == 0)
            {
                break;
            }
            else if (highlight == 1)
            {
                return 0;
            }
        }
    }

    clear();
    keypad(stdscr, FALSE);
    echo();
    mvprintw(1, 2, "%s", "Enter your user name: ");
    refresh();
    char *username = (char *)calloc(100, sizeof(char));
    char *password = (char *)calloc(30, sizeof(char));
    char *email = (char *)calloc(50, sizeof(char));

    scanw(" %s", username);

    if (is_available(username))
    {
        clear();
        mvprintw(16, 35, "%s", "This username exist!");
        refresh();
        napms(2000);
        free(username);
        return create_user();
    }

    if (strlen(username) > 30)
    {
        clear();
        mvprintw(15, 35, "%s", "This username is too long!");
        refresh();
        napms(1500);
        free(username);
        return create_user();
    }

    if (strlen(username) <= 3)
    {
        clear();
        mvprintw(15, 35, "%s", "This username is too short!");
        refresh();
        napms(1500);
        free(username);
        return create_user();
    }

    clear();
    time_t start = time(NULL);
    mvprintw(16, 10, "%s", "If you want a random password, please press Enter button in 3 seconds.");
    refresh();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    int button = 100;
    int breaked = 0;

    while (button != 10)
    {
        time_t current = time(NULL);
        if (difftime(current, start) > 3.0)
        {
            breaked = 1;
            break;
        }
        else if (difftime(current, start) > 2.0 && difftime(current, start) <= 3.0)
        {
            mvprintw(16, 10, "%s", "If you want a random password, please press Enter button in 1 seconds.");
            refresh();
        }
        else if (difftime(current, start) > 1.0 && difftime(current, start) <= 2.0)
        {
            mvprintw(16, 10, "%s", "If you want a random password, please press Enter button in 2 seconds.");
            refresh();
        }
        button = getch();
    }
    clear();
    echo();
    keypad(stdscr, FALSE);
    nodelay(stdscr, FALSE);
    if (breaked == 0)
    {
        srand(time(NULL));
        int min = 10, max = 20;
        int random_length = (rand() % (max - min) + 1) + min;

        password = (char *)realloc(password, sizeof(char) * (random_length + 1));
        password[random_length] = '\0';
        for (int q = 0; q < random_length; q++)
        {
            password[q] = ' ';
        }

        min = 48;
        max = 57;
        char random_digit = (char)(rand() % (max - min) + 1) + min;

        min = 65;
        max = 90;
        char random_upper = (char)(rand() % (max - min) + 1) + min;

        min = 97;
        max = 122;
        char random_lower = (char)(rand() % (max - min) + 1) + min;

        min = 0;
        max = random_length - 1;
        int random_index1 = (rand() % (max - min) + 1) + min;
        int random_index2 = (rand() % (max - min) + 1) + min;
        while (random_index1 == random_index2)
        {
            random_index2 = (rand() % (max - min) + 1) + min;
        }
        int random_index3 = (rand() % (max - min) + 1) + min;
        while ((random_index1 == random_index3) || (random_index3 == random_index2))
        {
            random_index3 = (rand() % (max - min) + 1) + min;
        }
        password[random_index1] = random_digit;
        password[random_index2] = random_upper;
        password[random_index3] = random_lower;

        min = 33;
        max = 122;

        for (int k = 0; k < random_length; k++)
        {
            if (password[k] != ' ')
                continue;
            password[k] = (char)(rand() % (max - min) + 1) + min;
        }

        clear();
        mvprintw(15, 27, "This is your password: %s", password);
        mvprintw(16, 27, "%s", "Please press Enter button to go to next step.");
        noecho();
        keypad(stdscr, TRUE);
        int button_2;
        while (1)
        {
            button_2 = getch();
            if (button_2 == 10)
                break;
        }
        keypad(stdscr, FALSE);
        echo();
    }
    else
    {
        mvprintw(1, 2, "%s", "Enter your password: ");
        refresh();
        scanw(" %s", password);

        if (valid_password(password) == 0)
        {
            clear();
            mvprintw(15, 35, "%s", "This password is invalid!");
            refresh();
            napms(1500);
            free(username);
            free(password);
            return create_user();
        }
    }

    clear();
    mvprintw(1, 2, "%s", "Enter your email address: ");
    refresh();
    scanw(" %s", email);

    if (valid_email(email) == 0)
    {
        clear();
        mvprintw(15, 35, "%s", "This email is invalid!");
        refresh();
        napms(1500);
        free(username);
        free(password);
        free(email);
        return create_user();
    }

    char *file_login_path = (char *)malloc(100 * sizeof(char));
    char *file_setting_path = (char *)malloc(100 * sizeof(char));
    char file_scoreboard_path[] = "scoreboard.bin";
    sprintf(file_login_path, "%s/%s_login.txt", username, username);
    sprintf(file_setting_path, "%s/%s_setting.txt", username, username);
    mkdir(username, 0777);

    FILE *file = fopen(file_login_path, "w");

    fprintf(file, "%s\n", username);
    fprintf(file, "%s\n", password);
    fprintf(file, "%s\n", email);

    fclose(file);

    FILE *file2 = fopen(file_setting_path, "w");

    fprintf(file2, "%d\n", 1);
    // easy-med-hard
    fprintf(file2, "%d\n", 1);
    // color

    fclose(file2);

    ++main_scoreboard.num_users;
    strcpy(main_scoreboard.usernames[main_scoreboard.num_users - 1], username);
    FILE *file3 = fopen(file_scoreboard_path, "wb");
    fwrite(&main_scoreboard, sizeof(users_scoreboard), 1, file3);
    fclose(file3);

    clear();
    mvprintw(15, 35, "%s", "New user added successfully");
    refresh();
    napms(1500);
    free(username);
    free(password);
    free(email);
    free(file_login_path);
    free(file_setting_path);

    return 0;
}

int login()
{
    clear();
    char *options[] = {"I have an account", "Login as a guest", "Exit"};
    int option_numbers = 3;
    int highlight = 0;
    int ch;
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    while (1)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (highlight == i)
            {
                attron(A_REVERSE);
                attron(A_BOLD);
                mvprintw(2 * i + 1, 2, "%s", options[i]);
                attroff(A_BOLD);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(2 * i + 1, 2, "%s", options[i]);
            }
        }
        refresh();
        ch = getch();

        if (ch == KEY_DOWN)
        {
            if (highlight < 2)
                ++highlight;
        }
        else if (ch == KEY_UP)
        {
            if (highlight > 0)
                --highlight;
        }
        else if (ch == 10)
        {
            if (highlight == 0)
            {
                keypad(stdscr, FALSE);
                echo();
                clear();
                mvprintw(1, 2, "%s", "Enter your username: ");
                char *username = (char *)malloc(sizeof(char) * 31);
                wgetnstr(stdscr, username, 31);

                struct stat st;
                if (stat(username, &st) == 0 && S_ISDIR(st.st_mode))
                {
                    clear();
                    mvprintw(1, 2, "%s", "Enter your password: ");
                    char *password = (char *)malloc(sizeof(char) * 31);
                    wgetnstr(stdscr, password, 31);

                    char *file_path = (char *)malloc(sizeof(char) * 100);

                    sprintf(file_path, "%s/%s_login.txt", username, username);
                    FILE *file;
                    file = fopen(file_path, "r");
                    char *line_2 = (char *)malloc(sizeof(char) * 31);

                    fgets(line_2, 31, file);
                    fgets(line_2, 31, file);

                    line_2[strcspn(line_2, "\n")] = '\0';

                    if (strcmp(line_2, password) == 0)
                    {
                        clear();
                        mvprintw(16, 33, "You are logged in as %s :)", username);
                        refresh();

                        char *file_setting_path = (char *)malloc(100 * sizeof(char));
                        sprintf(file_setting_path, "%s/%s_setting.txt", username, username);
                        int level, color;
                        FILE *filepointer = fopen(file_setting_path, "r");

                        fscanf(filepointer, "%d", &level);
                        fscanf(filepointer, "%d", &color);

                        GENERAL_SETTING.LEVEL = level;
                        GENERAL_SETTING.COLOR = color;
                        strcpy(GENERAL_SETTING.USERNAME, username);

                        fclose(filepointer);

                        napms(2000);
                        return 0;
                    }
                    else
                    {
                        clear();
                        mvprintw(16, 33, "%s", "Incorrect password :(");
                        refresh();
                        napms(1500);
                        clear();
                        time_t start = time(NULL);
                        mvprintw(16, 10, "%s", "If you want to restore your password, Enter the cheat key in 3 seconds.");
                        refresh();
                        noecho();
                        keypad(stdscr, TRUE);
                        nodelay(stdscr, TRUE);
                        int cheat = 10;
                        int breaked = 0;

                        while (cheat != KEY_DOWN)
                        {
                            time_t current = time(NULL);
                            if (difftime(current, start) > 3.0)
                            {
                                breaked = 1;
                                break;
                            }
                            else if (difftime(current, start) > 2.0 && difftime(current, start) <= 3.0)
                            {
                                mvprintw(16, 10, "%s", "If you want to restore your password, Enter the cheat key in 1 seconds.");
                                refresh();
                            }
                            else if (difftime(current, start) > 1.0 && difftime(current, start) <= 2.0)
                            {
                                mvprintw(16, 10, "%s", "If you want to restore your password, Enter the cheat key in 2 seconds.");
                                refresh();
                            }
                            cheat = getch();
                        }

                        echo();
                        keypad(stdscr, FALSE);
                        nodelay(stdscr, FALSE);
                        if (breaked == 0)
                        {
                            clear();
                            mvprintw(1, 2, "This is your password: %s\n  Now you can login again.", line_2);
                            refresh();
                            napms(5000);
                        }

                        return login();
                    }

                    fclose(file);
                    free(username);
                    free(password);
                    free(line_2);
                }
                else
                {
                    clear();
                    mvprintw(16, 35, "%s", "This username dosen't exist.");
                    refresh();
                    napms(1500);
                    free(username);
                    return login();
                }
            }
            else if (highlight == 1)
            {
                clear();
                mvprintw(16, 33, "%s", "You are logged in as Guest :-|");
                refresh();
                initialize_general_setting();
                napms(1500);
                return 0;
            }
            else if (highlight == 2)
            {
                return 0;
            }
        }
    }
}

//////////////////menu
int game_menu()
{
    clear();
    char *options[] = {"Make a new game", "Resume the last game", "Scoreboard", "Setting", "Exit"};
    int option_numbers = 5;
    int highlight = 0;
    int ch;
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    while (1)
    {
        for (int i = 0; i < 5; ++i)
        {
            if (highlight == i)
            {
                attron(A_REVERSE);
                attron(A_BOLD);
                mvprintw(2 * i + 1, 2, "%s", options[i]);
                attroff(A_BOLD);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(2 * i + 1, 2, "%s", options[i]);
            }
        }
        refresh();
        ch = getch();

        if (ch == KEY_DOWN)
        {
            if (highlight < 4)
                ++highlight;
        }
        else if (ch == KEY_UP)
        {
            if (highlight > 0)
                --highlight;
        }
        else if (ch == 10)
        {
            if (highlight == 0)
            {
                return make_new_game(); // make_new_game();
            }
            else if (highlight == 1)
            {
                if (strcmp(GENERAL_SETTING.USERNAME, "Guest") == 0)
                {
                    clear();
                    mvprintw(16, 37, "%s", "You must login first!");
                    refresh();
                    napms(1500);
                    clear();
                }
                else
                {
                    char *path = (char *)malloc(50 * sizeof(char));
                    sprintf(path, "%s/map.bin", GENERAL_SETTING.USERNAME);
                    FILE *saved_game = fopen(path, "rb");
                    if (saved_game == NULL)
                    {
                        clear();
                        mvprintw(16, 37, "%s", "The last game isn't available!");
                        refresh();
                        napms(1500);
                        clear();
                    }
                    else
                    {
                        return resume_game();
                    }
                }
            }
            else if (highlight == 2)
            {
                return scoreboard();
            }
            else if (highlight == 3)
            {
                return setting();
            }
            else if (highlight == 4)
            {
                return 0;
            }
        }
    }
}

int setting()
{

    clear();
    char *options[] = {"Change user setting", "Change Guest setting", "Exit"};
    int option_numbers = 3;
    int highlight = 0;
    int ch;
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    while (1)
    {
        mvprintw(31, 33, "You are logged in as %s", GENERAL_SETTING.USERNAME);
        for (int i = 0; i < 3; ++i)
        {
            if (highlight == i)
            {
                attron(A_REVERSE);
                attron(A_BOLD);
                mvprintw(2 * i + 1, 2, "%s", options[i]);
                attroff(A_BOLD);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(2 * i + 1, 2, "%s", options[i]);
            }
        }
        refresh();
        ch = getch();

        if (ch == KEY_DOWN)
        {
            if (highlight < 2)
                ++highlight;
        }
        else if (ch == KEY_UP)
        {
            if (highlight > 0)
                --highlight;
        }
        else if (ch == 10)
        {
            if (highlight == 0)
            {

                if (strcmp(GENERAL_SETTING.USERNAME, "Guest") == 0)
                {
                    clear();
                    mvprintw(16, 37, "%s", "Please login first");
                    refresh();
                    napms(1500);
                    return 0;
                }
                else
                {

                    char *file_setting_path = (char *)malloc(100 * sizeof(char));
                    sprintf(file_setting_path, "%s/%s_setting.txt", GENERAL_SETTING.USERNAME, GENERAL_SETTING.USERNAME);
                    FILE *file = fopen(file_setting_path, "w");

                    clear();
                    keypad(stdscr, FALSE);
                    mvprintw(1, 2, "%s", "Please select the level: ");
                    mvprintw(3, 2, "%s", "1 for Easy ");
                    mvprintw(5, 2, "%s", "2 for Normal ");
                    mvprintw(7, 2, "%s", "3 for Hard ");
                    refresh();

                    char button;
                    while (1)
                    {
                        button = getch();
                        if (button == '1' || button == '2' || button == '3')
                            break;
                    }

                    fprintf(file, "%d\n", button - 48);
                    clear();
                    mvprintw(16, 35, "%s", "The level changed successfully");
                    refresh();
                    napms(1500);
                    clear();

                    mvprintw(1, 2, "%s", "Please select your Hero color: ");
                    mvprintw(3, 2, "%s", "1 for Red ");
                    mvprintw(5, 2, "%s", "2 for Blue ");
                    mvprintw(7, 2, "%s", "3 for Yellow ");
                    mvprintw(9, 2, "%s", "4 for Green ");
                    refresh();

                    while (1)
                    {
                        button = getch();
                        if (button == '1' || button == '2' || button == '3' || button == '4')
                            break;
                    }

                    fprintf(file, "%d\n", button - 48);
                    clear();
                    mvprintw(16, 33, "%s", "The Hero color changed successfully");
                    refresh();
                    napms(1500);
                    clear();
                    fclose(file);

                    int level, color;
                    FILE *filepointer = fopen(file_setting_path, "r");

                    fscanf(filepointer, "%d", &level);
                    fscanf(filepointer, "%d", &color);

                    GENERAL_SETTING.LEVEL = level;
                    GENERAL_SETTING.COLOR = color;

                    return 0;
                }
            }
            else if (highlight == 1)
            {

                char file_setting_path[] = "setting.txt";
                FILE *file = fopen(file_setting_path, "w");

                clear();
                keypad(stdscr, FALSE);
                mvprintw(1, 2, "%s", "Please select the level: ");
                mvprintw(3, 2, "%s", "1 for Easy ");
                mvprintw(5, 2, "%s", "2 for Normal ");
                mvprintw(7, 2, "%s", "3 for Hard ");
                refresh();

                char button;
                while (1)
                {
                    button = getch();
                    if (button == '1' || button == '2' || button == '3')
                        break;
                }

                fprintf(file, "%d\n", button - 48);
                clear();
                mvprintw(16, 35, "%s", "The level changed successfully");
                refresh();
                napms(1500);
                clear();

                mvprintw(1, 2, "%s", "Please select your Hero color: ");
                mvprintw(3, 2, "%s", "1 for Red ");
                mvprintw(5, 2, "%s", "2 for Blue ");
                mvprintw(7, 2, "%s", "3 for Yellow ");
                mvprintw(9, 2, "%s", "4 for Green ");
                refresh();

                while (1)
                {
                    button = getch();
                    if (button == '1' || button == '2' || button == '3' || button == '4')
                        break;
                }

                fprintf(file, "%d\n", button - 48);
                clear();
                mvprintw(16, 35, "%s", "The Hero color changed successfully");
                refresh();
                napms(1500);
                clear();
                fclose(file);
                initialize_general_setting();
                return 0;
            }
            else if (highlight == 2)
            {
                return 0;
            }
        }
    }
}

int scoreboard()
{
    if (main_scoreboard.num_users == 0)
    {
        clear();
        mvprintw(16, 37, "%s", "There isn't any user yet!");
        refresh();
        napms(1500);
        return 0;
    }

    clear();
    keypad(stdscr, FALSE);

    user myusers[main_scoreboard.num_users];

    for (int i = 0; i < main_scoreboard.num_users; ++i)
    {
        myusers[i].experience = main_scoreboard.experience[i];
        myusers[i].gold = main_scoreboard.golds[i];
        myusers[i].num_games = main_scoreboard.num_played_games[i];
        myusers[i].score = main_scoreboard.score[i];
        strcpy(myusers[i].name, main_scoreboard.usernames[i]);
    }

    for (int i = 0; i < main_scoreboard.num_users - 1; ++i)
    {
        for (int j = 0; j < main_scoreboard.num_users - 1 - i; ++j)
        {
            if (myusers[j].score < myusers[j + 1].score)
            {
                user temp;
                temp = myusers[j];
                myusers[j] = myusers[j + 1];
                myusers[j + 1] = temp;
            }
        }
    }

    init_color(11, 750, 635, 0);
    init_color(12, 550, 550, 550);
    init_color(13, 600, 300, 150);

    init_pair(1, 11, COLOR_BLACK);
    init_pair(2, 12, COLOR_BLACK);
    init_pair(3, 13, COLOR_BLACK);

    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    int highlight = 0;

    int button;

    while (1)
    {

        for (int i = 0; i <= ((11 < main_scoreboard.num_users - 1) ? 11 : main_scoreboard.num_users - 1); i++)
        {
            if (strcmp(GENERAL_SETTING.USERNAME, myusers[i].name) == 0)
                attron(A_BOLD);
            if (highlight == i)
            {
                attron(A_REVERSE);
                if (i == 0)
                {

                    attron(COLOR_PAIR(1));
                    mvprintw(2 * i + 3, 3, "%s", "\U0001F947");
                    mvprintw(2 * i + 3, 7, "%d", i + 1);
                    attron(A_UNDERLINE);
                    int space_number = (30 - strlen(myusers[i].name)) / 2;
                    mvprintw(2 * i + 3, 12 + space_number, "%s", myusers[i].name);
                    attroff(A_UNDERLINE);
                    mvprintw(2 * i + 3, 46, "%d", myusers[i].score);
                    mvprintw(2 * i + 3, 53, "%d", myusers[i].gold);
                    mvprintw(2 * i + 3, 63, "%d", myusers[i].num_games);
                    mvprintw(2 * i + 3, 75, "%d", myusers[i].experience);
                    mvprintw(3, 83, "%s", "Goat");
                    attroff(COLOR_PAIR(1));
                }
                else if (i == 1)
                {

                    attron(COLOR_PAIR(2));
                    mvprintw(2 * i + 3, 3, "%s", "\U0001F948");
                    mvprintw(2 * i + 3, 7, "%d", i + 1);
                    int space_number = (30 - strlen(myusers[i].name)) / 2;
                    attron(A_UNDERLINE);
                    mvprintw(2 * i + 3, 12 + space_number, "%s", myusers[i].name);
                    attroff(A_UNDERLINE);
                    mvprintw(2 * i + 3, 46, "%d", myusers[i].score);
                    mvprintw(2 * i + 3, 53, "%d", myusers[i].gold);
                    mvprintw(2 * i + 3, 63, "%d", myusers[i].num_games);
                    mvprintw(2 * i + 3, 75, "%d", myusers[i].experience);
                    mvprintw(5, 83, "%s", "Legend");
                    attroff(COLOR_PAIR(2));
                }
                else if (i == 2)
                {

                    attron(COLOR_PAIR(3));
                    mvprintw(2 * i + 3, 3, "%s", "\U0001F949");
                    mvprintw(2 * i + 3, 7, "%d", i + 1);
                    int space_number = (30 - strlen(myusers[i].name)) / 2;
                    attron(A_UNDERLINE);
                    mvprintw(2 * i + 3, 12 + space_number, "%s", myusers[i].name);
                    attroff(A_UNDERLINE);
                    mvprintw(2 * i + 3, 46, "%d", myusers[i].score);
                    mvprintw(2 * i + 3, 53, "%d", myusers[i].gold);
                    mvprintw(2 * i + 3, 63, "%d", myusers[i].num_games);
                    mvprintw(2 * i + 3, 75, "%d", myusers[i].experience);
                    mvprintw(7, 83, "%s", "Hero");
                    attroff(COLOR_PAIR(3));
                }
                else
                {

                    mvprintw(2 * i + 3, 7, "%d", i + 1);
                    int space_number = (30 - strlen(myusers[i].name)) / 2;
                    mvprintw(2 * i + 3, 12 + space_number, "%s", myusers[i].name);
                    mvprintw(2 * i + 3, 46, "%d", myusers[i].score);
                    mvprintw(2 * i + 3, 53, "%d", myusers[i].gold);
                    mvprintw(2 * i + 3, 63, "%d", myusers[i].num_games);
                    mvprintw(2 * i + 3, 75, "%d", myusers[i].experience);
                }
                attroff(A_REVERSE);
            }
            else
            {
                if (i == 0)
                {

                    mvprintw(2 * i + 3, 3, "%s", "\U0001F947");
                    attron(COLOR_PAIR(1));
                    mvprintw(2 * i + 3, 7, "%d", i + 1);
                    int space_number = (30 - strlen(myusers[i].name)) / 2;
                    attron(A_UNDERLINE);
                    mvprintw(2 * i + 3, 12 + space_number, "%s", myusers[i].name);
                    attroff(A_UNDERLINE);
                    mvprintw(2 * i + 3, 46, "%d", myusers[i].score);
                    mvprintw(2 * i + 3, 53, "%d", myusers[i].gold);
                    mvprintw(2 * i + 3, 63, "%d", myusers[i].num_games);
                    mvprintw(2 * i + 3, 75, "%d", myusers[i].experience);
                    mvprintw(3, 83, "%s", "Goat");
                    attroff(COLOR_PAIR(1));
                }
                else if (i == 1)
                {

                    mvprintw(2 * i + 3, 3, "%s", "\U0001F948");
                    attron(COLOR_PAIR(2));
                    mvprintw(2 * i + 3, 7, "%d", i + 1);
                    int space_number = (30 - strlen(myusers[i].name)) / 2;
                    attron(A_UNDERLINE);
                    mvprintw(2 * i + 3, 12 + space_number, "%s", myusers[i].name);
                    attroff(A_UNDERLINE);
                    mvprintw(2 * i + 3, 46, "%d", myusers[i].score);
                    mvprintw(2 * i + 3, 53, "%d", myusers[i].gold);
                    mvprintw(2 * i + 3, 63, "%d", myusers[i].num_games);
                    mvprintw(2 * i + 3, 75, "%d", myusers[i].experience);
                    mvprintw(5, 83, "%s", "Legend");
                    attroff(COLOR_PAIR(2));
                }
                else if (i == 2)
                {

                    mvprintw(2 * i + 3, 3, "%s", "\U0001F949");
                    attron(COLOR_PAIR(3));
                    mvprintw(2 * i + 3, 7, "%d", i + 1);
                    int space_number = (30 - strlen(myusers[i].name)) / 2;
                    attron(A_UNDERLINE);
                    mvprintw(2 * i + 3, 12 + space_number, "%s", myusers[i].name);
                    attroff(A_UNDERLINE);
                    mvprintw(2 * i + 3, 46, "%d", myusers[i].score);
                    mvprintw(2 * i + 3, 53, "%d", myusers[i].gold);
                    mvprintw(2 * i + 3, 63, "%d", myusers[i].num_games);
                    mvprintw(2 * i + 3, 75, "%d", myusers[i].experience);
                    mvprintw(7, 83, "%s", "Hero");
                    attroff(COLOR_PAIR(3));
                }
                else
                {

                    mvprintw(2 * i + 3, 7, "%d", i + 1);
                    int space_number = (30 - strlen(myusers[i].name)) / 2;
                    mvprintw(2 * i + 3, 12 + space_number, "%s", myusers[i].name);
                    mvprintw(2 * i + 3, 46, "%d", myusers[i].score);
                    mvprintw(2 * i + 3, 53, "%d", myusers[i].gold);
                    mvprintw(2 * i + 3, 63, "%d", myusers[i].num_games);
                    mvprintw(2 * i + 3, 75, "%d", myusers[i].experience);
                }
            }
            if (strcmp(GENERAL_SETTING.USERNAME, myusers[i].name) == 0)
                attroff(A_BOLD);
        }

        attron(A_BOLD);
        mvprintw(1, 5, "%s", " rank ");                       // 5-11
        mvprintw(1, 12, "%s", "           username        "); // 12-42
        mvprintw(1, 43, "%s", " score ");                     // 43-50
        mvprintw(1, 51, "%s", " gold ");                      // 51-57
        mvprintw(1, 58, "%s", " num_games ");                 // 58-69
        mvprintw(1, 69, "%s", "  experience ");               // 70-82
        mvprintw(31, 33, "%s", "Press Enter to return to menu");
        attroff(A_BOLD);

        refresh();

        button = getch();

        if (button == KEY_DOWN)
        {
            if (highlight < ((11 < main_scoreboard.num_users - 1) ? 11 : main_scoreboard.num_users - 1))
                ++highlight;
        }
        else if (button == KEY_UP)
        {
            if (highlight > 0)
                --highlight;
        }
        else if (button == 10)
        {
            return 0;
        }
    }
}

int main_menu()
{
    clear();
    char *options[] = {"Create a new user", "Login", "Game menu", "setting", "Scoreboard", "Profile", "Exit"};
    int option_numbers = 7;
    int highlight = 0;
    int ch;
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    while (1)
    {
        for (int i = 0; i < 7; ++i)
        {
            if (highlight == i)
            {
                attron(A_REVERSE);
                attron(A_BOLD);
                mvprintw(2 * i + 1, 2, "%s", options[i]);
                attroff(A_BOLD);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(2 * i + 1, 2, "%s", options[i]);
            }
        }
        refresh();
        ch = getch();

        if (ch == KEY_DOWN)
        {
            if (highlight < 6)
                ++highlight;
        }
        else if (ch == KEY_UP)
        {
            if (highlight > 0)
                --highlight;
        }
        else if (ch == 10)
        {
            if (highlight == 0)
            {
                create_user();
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
            }
            else if (highlight == 1)
            {
                login();
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
            }
            else if (highlight == 2)
            {
                game_menu();
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
            }
            else if (highlight == 3)
            {
                setting();
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
            }
            else if (highlight == 4)
            {
                scoreboard();
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
            }
            else if (highlight == 5)
            {
                profile_menu();
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
            }
            else if (highlight == 6)
            {
                endwin();
                return 0;
            }
        }
    }
}

void initialize_general_setting()
{

    strcpy(GENERAL_SETTING.USERNAME, "Guest");
    FILE *pointer = fopen("setting.txt", "r");
    char level[10];
    char color[10];

    fgets(level, 3, pointer);
    fgets(color, 3, pointer);

    GENERAL_SETTING.LEVEL = atoi(level);
    GENERAL_SETTING.COLOR = atoi(color);

    main_map.floor = 1;
}

void setup_player(player *p)
{
    strcpy(p->username, GENERAL_SETTING.USERNAME);
    p->last_shot = 0;
    p->black_golds = 0;
    p->usual_golds = 0;
    p->number_of_health_enchant = 0;
    p->number_of_speed_enchant = 0;
    p->number_of_damage_enchant = 0;
    p->number_of_foods = 0;
    p->number_of_master_keys = 0;
    p->number_of_broken_master_keys = 0;
    p->num_wrong_password = 0;
    p->is_damage_enchant_active = 0;
    p->is_health_enchant_active = 0;
    p->is_speed_enchant_active = 0;

    p->damage_enchant_counter = 0;
    p->health_enchant_counter = 0;
    p->speed_enchant_counter = 0;

    p->number_of_mace = 1;
    p->number_of_arrow = 0;
    p->number_of_dagger = 0;
    p->number_of_sword = 0;
    p->number_of_wand = 0;

    for (int i = 0; i < 7; ++i)
    {
        p->showed_room[i] = 0;
    }

    p->default_ammo = 1;

    for (int i = 0; i < 92; ++i)
    {
        for (int j = 0; j < 32; ++j)
        {
            p->visited[i][j] = 0;
        }
    }

    p->last_attack = time(NULL);
    p->last_password = time(NULL);
    p->last_time = time(NULL);

    switch (GENERAL_SETTING.LEVEL)
    {
    case 1:
    {
        p->initial_health = 100;
        p->initial_hunger = 100;
    }

    break;

    case 2:
        p->initial_health = 75;
        p->initial_hunger = 75;
        break;

    case 3:
        p->initial_health = 50;
        p->initial_hunger = 50;
        break;
    }

    p->health = p->initial_health;
    p->hunger = 0;
}

void initialize_random_position(player *p)
{
    int random_room_number = (rand() % main_map.num_rooms);
    int found = 0;

    for (int i = main_map.rooms[random_room_number].start.x;
         i < main_map.rooms[random_room_number].start.x + main_map.rooms[random_room_number].width;
         ++i)
    {
        for (int j = main_map.rooms[random_room_number].start.y;
             j < main_map.rooms[random_room_number].start.y + main_map.rooms[random_room_number].height;
             ++j)
        {

            if (main_map.signed_map[i][j] == '<' || main_map.signed_map[i][j] == 'T')
            {
                random_room_number = (random_room_number == 0) ? 1 : random_room_number - 1;
                found = 1;
                break;
            }
        }
        if (found == 1)
            break;
    }

    point random_position;
    int counter = 0;

    while (1)
    {
        if (counter == 3)
            break;
        random_position.x = (rand() % (main_map.rooms[random_room_number].width - 1)) + main_map.rooms[random_room_number].start.x + 1;
        random_position.y = (rand() % (main_map.rooms[random_room_number].height - 1)) + main_map.rooms[random_room_number].start.y + 1;

        if (main_map.signed_map[random_position.x][random_position.y] != '.')
        {
            ++counter;
            continue;
        }
        int same_location = 0;
        for (int k = 0; k < main_map.num_enemies; ++k)
        {
            if (enemies[k].position.x == random_position.x && enemies[k].position.y == random_position.y)
            {
                same_location = 1;
                break;
            }
        }

        if (same_location == 0)
        {
            p->position.x = random_position.x;
            p->position.y = random_position.y;
            break;
        }
    }

    p->last_time = time(NULL);

    if (counter == 3)
        initialize_random_position(p);

    else
        p->showed_room[random_room_number] = 1;
}

int inverse(int number)
{
    int r_number = 0;

    while (number != 0)
    {
        int digit = number % 10;
        r_number = r_number * 10 + digit;
        number /= 10;
    }

    return r_number;
}

int player_movement(int ch, int pick_up)
{
    int delta[3] = {1, 0, -1};
    int dx = 0;
    int dy = 0;

    if (ch == 'f')
    {
        int breaked = 0;
        int sth = 10;
        nodelay(stdscr, TRUE);
        time_t current, start;
        start = time(NULL);

        while (sth != 'y' && sth != 'Y' && sth != 'u' && sth != 'U' && sth != 'h' && sth != 'H' && sth != 'j' && sth != 'J' && sth != 'k' && sth != 'K' && sth != 'l' && sth != 'L' && sth != 'b' && sth != 'B' && sth != 'n' && sth != 'N')
        {
            current = time(NULL);
            if (difftime(current, start) > 3.0)
            {
                breaked = 1;
                break;
            }

            sth = getch();
            if (sth != ERR)
            {
                break;
            }

            current = time(NULL);
            if (difftime(current, start) > 3.0)
            {
                breaked = 1;
                break;
            }
        }

        nodelay(stdscr, FALSE);

        sth = (sth < 91) ? (sth + 32) : sth;

        if (breaked == 0)
        {
            if (sth == 'y')
            {
                dx = delta[2];
                dy = delta[2];
            }
            else if (sth == 'u')
            {
                dx = delta[0];
                dy = delta[2];
            }
            else if (sth == 'h')
            {
                dx = delta[2];
                dy = delta[1];
            }
            else if (sth == 'j')
            {
                dx = delta[1];
                dy = delta[2];
            }
            else if (sth == 'k')
            {
                dx = delta[1];
                dy = delta[0];
            }
            else if (sth == 'l')
            {
                dx = delta[0];
                dy = delta[1];
            }
            else if (sth == 'b')
            {
                dx = delta[2];
                dy = delta[0];
            }
            else if (sth == 'n')
            {
                dx = delta[0];
                dy = delta[0];
            }
            else
            {
                return 1;
            }

            while (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != '+' &&
                   main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != 'R' &&
                   main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != 'G' &&
                   main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != 'h' &&
                   main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != '-' &&
                   main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != '|' &&
                   main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != 'o' &&
                   main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != 't' &&
                   main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != '^' &&
                   main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != '<' &&
                   main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != 'T' &&
                   main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] != ' ')
            {
                if ((0 <= main_player.position.x + dx) && (92 > main_player.position.x + dx) && (0 < main_player.position.y + dy) && (32 > main_player.position.y + dy))
                {
                    int same_location = 0;

                    for (int x = 0; x < main_map.num_enemies; ++x)
                    {
                        if ((enemies[x].position.x == main_player.position.x + dx) && (enemies[x].position.y == main_player.position.y + dy))
                        {
                            same_location = 1;
                            break;
                        }
                    }

                    if (same_location == 1)
                        break;
                    player_movement(sth, 1);
                }
                else
                    break;
            }
        }
    }
    else
    {
        if (main_player.speed_enchant_counter > 0)
            --main_player.speed_enchant_counter;
        if (main_player.speed_enchant_counter == 0)
            main_player.is_speed_enchant_active = 0;

        if (main_player.health_enchant_counter > 0)
            --main_player.health_enchant_counter;
        if (main_player.health_enchant_counter == 0)
            main_player.is_health_enchant_active = 0;

        if (main_player.damage_enchant_counter > 0)
            --main_player.damage_enchant_counter;
        if (main_player.damage_enchant_counter == 0)
            main_player.is_damage_enchant_active = 0;

        clear();

        if (ch == 'y')
        {
            dx = delta[2];
            dy = delta[2];
        }
        else if (ch == 'u')
        {
            dx = delta[0];
            dy = delta[2];
        }
        else if (ch == 'h')
        {
            dx = delta[2];
            dy = delta[1];
        }
        else if (ch == 'j')
        {
            dx = delta[1];
            dy = delta[2];
        }
        else if (ch == 'k')
        {
            dx = delta[1];
            dy = delta[0];
        }
        else if (ch == 'l')
        {
            dx = delta[0];
            dy = delta[1];
        }
        else if (ch == 'b')
        {
            dx = delta[2];
            dy = delta[0];
        }
        else if (ch == 'n')
        {
            dx = delta[0];
            dy = delta[0];
        }

        int same_location = 0;

        for (int x = 0; x < main_map.num_enemies; ++x)
        {
            if ((enemies[x].position.x == main_player.position.x + dx) && (enemies[x].position.y == main_player.position.y + dy))
            {
                same_location = 1;
                break;
            }
        }

        if (same_location)
            return 1;

        if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'M')
        {
            if (pick_up == 1)
            {

                if (rand() % 10 == 0)
                {
                    ++main_player.number_of_broken_master_keys;
                    attron(COLOR_PAIR(1));
                    mvprintw(1, 1, "%s", "A broken master key added to your backpack");
                    attroff(COLOR_PAIR(1));
                }
                else
                {
                    ++main_player.number_of_master_keys;
                    attron(COLOR_PAIR(1));
                    mvprintw(1, 1, "%s", "A master key added to your backpack");
                    attroff(COLOR_PAIR(1));
                }
                main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] = '.';
            }

            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'h')
        {
            if (main_map.signed_map[main_player.position.x][main_player.position.y] == '#')
            {
                attron(COLOR_PAIR(6));
                mvprintw(1, 1, "%s", "You enterd a new room");
                attroff(COLOR_PAIR(6));
            }
            refresh();
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            main_player.num_wrong_password = 0;
            main_player.password = 1;
            main_player.has_password = 0;
            main_player.showed_password = 1;

            main_player.showed_room[recognize_rooms(main_player.position)] = 1;
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'R')
        {
            print_game();
            int sth = getch();
            if (sth == 'p' || sth == 'P')
            {
                if (main_player.has_password == 1)
                {
                    mvprintw(1, 35, "%s", "Enter the password:");
                    refresh();
                    print_game();

                    move(1, 54);
                    attron(COLOR_PAIR(8));
                    echo();
                    attroff(COLOR_PAIR(8));
                    int ch = getch();
                    noecho();
                    if ((ch - '0') != (main_player.password / 1000))
                    {
                        mvprintw(1, 35, "%s", "                              ");
                        refresh();
                        attron(COLOR_PAIR(5));
                        mvprintw(1, 38, "%s", "wrong password");
                        attroff(COLOR_PAIR(5));
                        refresh();
                        napms(1500);
                        clear();
                        ++main_player.num_wrong_password;
                        if (main_player.num_wrong_password == 3)
                        {
                            main_player.num_wrong_password = 0;
                            main_player.password = 1;
                            main_player.has_password = 0;
                            main_player.showed_password = 1;
                        }
                        refresh();
                    }
                    else
                    {
                        attron(COLOR_PAIR(8));
                        echo();
                        attroff(COLOR_PAIR(8));
                        ch = getch();
                        noecho();
                        if ((ch - '0') != (main_player.password % 1000) / 100)
                        {
                            mvprintw(1, 35, "%s", "                           ");
                            refresh();
                            attron(COLOR_PAIR(5));
                            mvprintw(1, 38, "%s", "wrong password");
                            attroff(COLOR_PAIR(5));
                            refresh();
                            napms(1500);
                            clear();
                            ++main_player.num_wrong_password;
                            if (main_player.num_wrong_password == 3)
                            {
                                main_player.num_wrong_password = 0;
                                main_player.password = 1;
                                main_player.has_password = 0;
                                main_player.showed_password = 1;
                            }
                            refresh();
                        }
                        else
                        {
                            attron(COLOR_PAIR(8));
                            echo();
                            attroff(COLOR_PAIR(8));
                            ch = getch();
                            noecho();
                            if ((ch - '0') != (main_player.password % 100) / 10)
                            {
                                mvprintw(1, 35, "%s", "                               ");
                                refresh();
                                attron(COLOR_PAIR(5));
                                mvprintw(1, 38, "%s", "wrong password");
                                attroff(COLOR_PAIR(5));
                                refresh();
                                napms(1500);
                                clear();
                                ++main_player.num_wrong_password;
                                if (main_player.num_wrong_password == 3)
                                {
                                    main_player.num_wrong_password = 0;
                                    main_player.password = 1;
                                    main_player.has_password = 0;
                                    main_player.showed_password = 1;
                                }
                                refresh();
                            }
                            else
                            {
                                attron(COLOR_PAIR(8));
                                echo();
                                attroff(COLOR_PAIR(8));
                                ch = getch();
                                noecho();
                                if ((ch - '0') != (main_player.password % 10))
                                {
                                    mvprintw(1, 35, "%s", "                            ");
                                    refresh();
                                    attron(COLOR_PAIR(5));
                                    mvprintw(1, 38, "%s", "wrong password");
                                    attroff(COLOR_PAIR(5));
                                    refresh();
                                    napms(1500);
                                    clear();
                                    print_game();
                                    ++main_player.num_wrong_password;
                                    if (main_player.num_wrong_password == 3)
                                    {
                                        main_player.num_wrong_password = 0;
                                        main_player.password = 1;
                                        main_player.has_password = 0;
                                        main_player.showed_password = 1;
                                    }
                                    refresh();
                                }
                                else
                                {
                                    noecho();
                                    mvprintw(1, 35, "%s", "                               ");
                                    refresh();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "You unlocked the door with password");
                                    attroff(COLOR_PAIR(6));
                                    main_player.position.x += dx;
                                    main_player.position.y += dy;
                                    main_player.visited[main_player.position.x][main_player.position.y] = 1;
                                    main_map.signed_map[main_player.position.x][main_player.position.y] = 'G';
                                    main_player.num_wrong_password = 0;
                                    main_player.password = 1;
                                    main_player.has_password = 0;
                                    main_player.showed_password = 1;
                                    main_player.showed_room[recognize_rooms(main_player.position)] = 1;
                                }
                            }
                        }
                    }
                }
                else
                {
                    mvprintw(1, 35, "%s", "                  ");
                    refresh();
                    attron(COLOR_PAIR(5));
                    mvprintw(1, 35, "%s", "You don't have any password");
                    attroff(COLOR_PAIR(5));
                }
            }
            else
            {

                if (main_player.number_of_broken_master_keys >= 2)
                {
                    main_player.number_of_broken_master_keys -= 2;
                    attron(COLOR_PAIR(6));
                    mvprintw(1, 1, "%s", "You unlocked the door with master key");
                    attroff(COLOR_PAIR(6));
                    main_player.position.x += dx;
                    main_player.position.y += dy;
                    main_player.visited[main_player.position.x][main_player.position.y] = 1;
                    main_map.signed_map[main_player.position.x][main_player.position.y] = 'G';
                    main_player.num_wrong_password = 0;
                    main_player.password = 1;
                    main_player.has_password = 0;
                    main_player.showed_password = 1;
                    main_player.showed_room[recognize_rooms(main_player.position)] = 1;
                }
                else if (main_player.number_of_master_keys >= 1)
                {
                    main_player.number_of_master_keys -= 1;
                    attron(COLOR_PAIR(6));
                    mvprintw(1, 1, "%s", "You unlocked the door with master key");
                    attroff(COLOR_PAIR(6));
                    main_player.position.x += dx;
                    main_player.position.y += dy;
                    main_player.visited[main_player.position.x][main_player.position.y] = 1;
                    main_map.signed_map[main_player.position.x][main_player.position.y] = 'G';
                    main_player.num_wrong_password = 0;
                    main_player.password = 1;
                    main_player.has_password = 0;
                    main_player.showed_password = 1;
                    main_player.showed_room[recognize_rooms(main_player.position)] = 1;
                }
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'G')
        {
            if (main_map.signed_map[main_player.position.x][main_player.position.y] == '#')
            {
                attron(COLOR_PAIR(6));
                mvprintw(1, 1, "%s", "You enterd a new room");
                attroff(COLOR_PAIR(6));
            }
            refresh();
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            main_player.num_wrong_password = 0;
            main_player.password = 1;
            main_player.has_password = 0;
            main_player.showed_password = 1;
            main_player.showed_room[recognize_rooms(main_player.position)] = 1;
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == '+')
        {
            if (main_map.signed_map[main_player.position.x][main_player.position.y] == '#')
            {
                attron(COLOR_PAIR(6));
                mvprintw(1, 1, "%s", "You enterd a new room");
                attroff(COLOR_PAIR(6));
            }
            refresh();
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            main_player.num_wrong_password = 0;
            main_player.password = 1;
            main_player.has_password = 0;
            main_player.showed_password = 1;
            main_player.showed_room[recognize_rooms(main_player.position)] = 1;
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == '-')
        {
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == '|')
        {
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 't' || main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == '^')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            main_map.signed_map[main_player.position.x][main_player.position.y] = '^';
            main_player.health -= 5;
            main_player.last_attack = time(NULL);
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == '#')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'o')
        {
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == '&')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            main_player.has_password = 1;
            main_player.last_password = time(NULL);
            main_player.showed_password = (rand() % (9000)) + 1000;
            if (rand() % 5 == 0)
            {
                main_player.password = inverse(main_player.showed_password);
            }
            else
            {
                main_player.password = main_player.showed_password;
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'g')
        {

            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;

            if (pick_up == 1)
            {
                main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                ++main_player.usual_golds;
                attron(COLOR_PAIR(1));
                mvprintw(1, 1, "%s", "You collected a gold");
                attroff(COLOR_PAIR(1));
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'b')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;

            if (pick_up == 1)
            {
                main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                ++main_player.black_golds;
                attron(COLOR_PAIR(1));
                mvprintw(1, 1, "%s", "You collected black a gold");
                attroff(COLOR_PAIR(1));
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'H')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            if (pick_up == 1)
            {
                main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                ++main_player.number_of_health_enchant;
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'S')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            if (pick_up == 1)
            {
                main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                ++main_player.number_of_speed_enchant;
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'D')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            if (pick_up == 1)
            {
                main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                ++main_player.number_of_damage_enchant;
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'd')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            if (pick_up == 1)
            {
                main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                main_player.number_of_dagger += 10;
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == '6')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            if (pick_up == 1)
            {
                main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                main_player.number_of_arrow += 1;
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == '7')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            if (pick_up == 1)
            {
                if (recognize_rooms(main_player.position) == -1)
                {
                    main_map.signed_map[main_player.position.x][main_player.position.y] = '#';
                }
                else
                {
                    main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                }
                main_player.number_of_dagger += 1;
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == '8')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            if (pick_up == 1)
            {
                if (recognize_rooms(main_player.position) == -1)
                {
                    main_map.signed_map[main_player.position.x][main_player.position.y] = '#';
                }
                else
                {
                    main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                }
                main_player.number_of_wand += 1;
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'w')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            if (pick_up == 1)
            {
                if (recognize_rooms(main_player.position) == -1)
                {
                    main_map.signed_map[main_player.position.x][main_player.position.y] = '#';
                }
                else
                {
                    main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                }
                main_player.number_of_wand += 8;
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'a')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            if (pick_up == 1)
            {
                main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                main_player.number_of_arrow += 20;
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 's')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            if (pick_up == 1)
            {
                main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                main_player.number_of_sword = 1;
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'f')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            if (pick_up == 1)
            {
                main_map.signed_map[main_player.position.x][main_player.position.y] = '.';
                if (main_player.hunger == 0)
                {
                    main_player.number_of_foods = (main_player.number_of_foods == 5) ? 5 : main_player.number_of_foods + 1;
                }
                else
                {
                    main_player.hunger = (main_player.hunger > 5) ? main_player.hunger - 5 : 0;
                }
            }
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == 'T')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
            main_player.num_wrong_password = 0;
            main_player.password = 1;
            main_player.has_password = 0;
            main_player.showed_password = 1;
            napms(500);
            draw_treasure_room();
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == '.')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
        }
        else if (main_map.signed_map[main_player.position.x + dx][main_player.position.y + dy] == '<')
        {
            main_player.position.x += dx;
            main_player.position.y += dy;
            main_player.visited[main_player.position.x][main_player.position.y] = 1;
        }
    }

    return 1; // pick up
}

int recognize_rooms(point p)
{
    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        if ((main_map.rooms[i].start.x <= p.x) &&
            (main_map.rooms[i].start.x + main_map.rooms[i].width >= p.x) &&
            (main_map.rooms[i].start.y <= p.y) &&
            (main_map.rooms[i].start.y + main_map.rooms[i].height >= p.y))
        {

            return i;
        }
    }

    return -1;
}

void print_wall()
{
    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        if (main_map.rooms[i].room_type == 1)
            attron(COLOR_PAIR(10));

        for (int j = main_map.rooms[i].start.x; j <= main_map.rooms[i].start.x + main_map.rooms[i].width; ++j)
        {
            for (int k = main_map.rooms[i].start.y; k <= main_map.rooms[i].start.y + main_map.rooms[i].height; ++k)
            {

                if (j == main_map.rooms[i].start.x && k == main_map.rooms[i].start.y)
                {
                    mvprintw(k + 2, j + 3, "%s", "\u250c");
                }
                else if (j == main_map.rooms[i].start.x && k == main_map.rooms[i].start.y + main_map.rooms[i].height)
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2514");
                }
                else if (j == main_map.rooms[i].start.x + main_map.rooms[i].width && k == main_map.rooms[i].start.y + main_map.rooms[i].height)
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2518");
                }
                else if (j == main_map.rooms[i].start.x + main_map.rooms[i].width && k == main_map.rooms[i].start.y)
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2510");
                }
                else if (main_map.signed_map[j][k] == '-')
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2500");
                }
                else if (main_map.signed_map[j][k] == '|')
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2502");
                }
            }
        }
        if (main_map.rooms[i].room_type == 1)
            attroff(COLOR_PAIR(10));
    }
}

void print_room(int i)
{
    if (main_map.rooms[i].room_type == 0)
    {

        for (int j = main_map.rooms[i].start.x; j <= main_map.rooms[i].start.x + main_map.rooms[i].width; ++j)
        {
            for (int k = main_map.rooms[i].start.y; k <= main_map.rooms[i].start.y + main_map.rooms[i].height; ++k)
            {

                if (j == main_map.rooms[i].start.x && k == main_map.rooms[i].start.y)
                {
                    mvprintw(k + 2, j + 3, "%s", "\u250c");
                }
                else if (j == main_map.rooms[i].start.x && k == main_map.rooms[i].start.y + main_map.rooms[i].height)
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2514");
                }
                else if (j == main_map.rooms[i].start.x + main_map.rooms[i].width && k == main_map.rooms[i].start.y + main_map.rooms[i].height)
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2518");
                }
                else if (j == main_map.rooms[i].start.x + main_map.rooms[i].width && k == main_map.rooms[i].start.y)
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2510");
                }
                else if (main_map.signed_map[j][k] == '-')
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2500");
                }
                else if (main_map.signed_map[j][k] == '|')
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2502");
                }
                else if (main_map.signed_map[j][k] == '+' || main_map.signed_map[j][k] == '^' ||
                         main_map.signed_map[j][k] == '<' || main_map.signed_map[j][k] == 'o')
                {
                    mvprintw(k + 2, j + 3, "%c", main_map.signed_map[j][k]);
                }
                else if (main_map.signed_map[j][k] == 't' || main_map.signed_map[j][k] == '.')
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2022");
                }
                else if (main_map.signed_map[j][k] == 'h')
                {
                    if (main_map.signed_map[j + 1][k] == '-' || main_map.signed_map[j - 1][k] == '-')
                    {
                        mvprintw(k + 2, j + 3, "%s", "\u2500");
                    }
                    else
                    {
                        mvprintw(k + 2, j + 3, "%s", "\u2502");
                    }
                }
                else if (main_map.signed_map[j][k] == 'M')
                {
                    attron(COLOR_PAIR(1));
                    mvprintw(k + 2, j + 3, "%s", "\u25B2");
                    attroff(COLOR_PAIR(1));
                }
                else if (main_map.signed_map[j][k] == 'g')
                {
                    attron(COLOR_PAIR(1));
                    mvprintw(k + 2, j + 3, "%s", "\u26c0");
                    attroff(COLOR_PAIR(1));
                }
                else if (main_map.signed_map[j][k] == 'b')
                {
                    attron(COLOR_PAIR(4));
                    mvprintw(k + 2, j + 3, "%s", "\u26c2");
                    attroff(COLOR_PAIR(4));
                }
                else if (main_map.signed_map[j][k] == 'H')
                {
                    attron(COLOR_PAIR(6));
                    mvprintw(k + 2, j + 3, "%s", "\u2695");
                    attroff(COLOR_PAIR(6));
                }
                else if (main_map.signed_map[j][k] == 'S')
                {
                    attron(COLOR_PAIR(8));
                    mvprintw(k + 2, j + 3, "%s", "\u26f7");
                    attroff(COLOR_PAIR(8));
                }
                else if (main_map.signed_map[j][k] == 'D')
                {
                    attron(COLOR_PAIR(4));
                    mvprintw(k + 2, j + 3, "%s", "\u2620");
                    attroff(COLOR_PAIR(4));
                }
                else if (main_map.signed_map[j][k] == 'd' || main_map.signed_map[j][k] == '7')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u2020");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 'w' || main_map.signed_map[j][k] == '8')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u269A");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 'a' || main_map.signed_map[j][k] == '6')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u27B3");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 's')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u2694");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 'f')
                {
                    attron(COLOR_PAIR(9));
                    mvprintw(k + 2, j + 3, "%s", "\u2299");
                    attroff(COLOR_PAIR(9));
                }
                else if (main_map.signed_map[j][k] == 'R')
                {
                    attron(COLOR_PAIR(5));
                    mvprintw(k + 2, j + 3, "%c", '@');
                    attroff(COLOR_PAIR(5));
                }
                else if (main_map.signed_map[j][k] == 'T')
                {
                    attron(COLOR_PAIR(1));
                    mvprintw(k + 2, j + 3, "%s", "\U0001f5dd");
                    attroff(COLOR_PAIR(1));
                }
                else if (main_map.signed_map[j][k] == 'G')
                {
                    attron(COLOR_PAIR(6));
                    mvprintw(k + 2, j + 3, "%c", '@');
                    attroff(COLOR_PAIR(6));
                }
                else if (main_map.signed_map[j][k] == '&')
                {
                    mvprintw(k + 2, j + 3, "%c", '&');
                }
            }
        }
    }
    else if (main_map.rooms[i].room_type == 1)
    {

        for (int j = main_map.rooms[i].start.x; j <= main_map.rooms[i].start.x + main_map.rooms[i].width; ++j)
        {
            for (int k = main_map.rooms[i].start.y; k <= main_map.rooms[i].start.y + main_map.rooms[i].height; ++k)
            {
                if (j == main_map.rooms[i].start.x && k == main_map.rooms[i].start.y)
                {
                    attron(COLOR_PAIR(10));
                    mvprintw(k + 2, j + 3, "%s", "\u250c");
                    attroff(COLOR_PAIR(10));
                }
                else if (j == main_map.rooms[i].start.x && k == main_map.rooms[i].start.y + main_map.rooms[i].height)
                {
                    attron(COLOR_PAIR(10));
                    mvprintw(k + 2, j + 3, "%s", "\u2514");
                    attroff(COLOR_PAIR(10));
                }
                else if (j == main_map.rooms[i].start.x + main_map.rooms[i].width && k == main_map.rooms[i].start.y + main_map.rooms[i].height)
                {
                    attron(COLOR_PAIR(10));
                    mvprintw(k + 2, j + 3, "%s", "\u2518");
                    attroff(COLOR_PAIR(10));
                }
                else if (j == main_map.rooms[i].start.x + main_map.rooms[i].width && k == main_map.rooms[i].start.y)
                {
                    attron(COLOR_PAIR(10));
                    mvprintw(k + 2, j + 3, "%s", "\u2510");
                    attroff(COLOR_PAIR(10));
                }
                else if (main_map.signed_map[j][k] == '-')
                {
                    attron(COLOR_PAIR(10));
                    mvprintw(k + 2, j + 3, "%s", "\u2500");
                    attroff(COLOR_PAIR(10));
                }
                else if (main_map.signed_map[j][k] == '|')
                {
                    attron(COLOR_PAIR(10));
                    mvprintw(k + 2, j + 3, "%s", "\u2502");
                    attroff(COLOR_PAIR(10));
                }
                else if (main_map.signed_map[j][k] == '+' || main_map.signed_map[j][k] == '^' ||
                         main_map.signed_map[j][k] == '<' || main_map.signed_map[j][k] == 'o')
                {
                    attron(COLOR_PAIR(10));
                    mvprintw(k + 2, j + 3, "%c", main_map.signed_map[j][k]);
                    attroff(COLOR_PAIR(10));
                }
                else if (main_map.signed_map[j][k] == 't' || main_map.signed_map[j][k] == '.')
                {
                    attron(COLOR_PAIR(10));
                    mvprintw(k + 2, j + 3, "%s", "\u2022");
                    attroff(COLOR_PAIR(10));
                }
                else if (main_map.signed_map[j][k] == 'h')
                {
                    if (main_map.signed_map[j + 1][k] == '-' || main_map.signed_map[j - 1][k] == '-')
                    {
                        attron(COLOR_PAIR(10));
                        mvprintw(k + 2, j + 3, "%s", "\u2500");
                        attroff(COLOR_PAIR(10));
                    }
                    else
                    {
                        attron(COLOR_PAIR(10));
                        mvprintw(k + 2, j + 3, "%s", "\u2502");
                        attroff(COLOR_PAIR(10));
                    }
                }
                else if (main_map.signed_map[j][k] == 'M')
                {
                    attron(COLOR_PAIR(1));
                    mvprintw(k + 2, j + 3, "%s", "\u25B2");
                    attroff(COLOR_PAIR(1));
                }
                else if (main_map.signed_map[j][k] == 'g')
                {
                    attron(COLOR_PAIR(1));
                    mvprintw(k + 2, j + 3, "%s", "\u26c0");
                    attroff(COLOR_PAIR(1));
                }
                else if (main_map.signed_map[j][k] == 'b')
                {
                    attron(COLOR_PAIR(4));
                    mvprintw(k + 2, j + 3, "%s", "\u26c2");
                    attroff(COLOR_PAIR(4));
                }
                else if (main_map.signed_map[j][k] == 'H')
                {
                    attron(COLOR_PAIR(6));
                    mvprintw(k + 2, j + 3, "%s", "\u2695");
                    attroff(COLOR_PAIR(6));
                }
                else if (main_map.signed_map[j][k] == 'S')
                {
                    attron(COLOR_PAIR(8));
                    mvprintw(k + 2, j + 3, "%s", "\u26f7");
                    attroff(COLOR_PAIR(8));
                }
                else if (main_map.signed_map[j][k] == 'D')
                {
                    attron(COLOR_PAIR(4));
                    mvprintw(k + 2, j + 3, "%s", "\u2620");
                    attroff(COLOR_PAIR(4));
                }
                else if (main_map.signed_map[j][k] == 'd' || main_map.signed_map[j][k] == '7')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u2020");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 'w' || main_map.signed_map[j][k] == '8')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u269A");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 'a' || main_map.signed_map[j][k] == '6')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u27B3");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 's')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u2694");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 'f')
                {
                    attron(COLOR_PAIR(9));
                    mvprintw(k + 2, j + 3, "%s", "\u2299");
                    attroff(COLOR_PAIR(9));
                }
                else if (main_map.signed_map[j][k] == 'R')
                {
                    attron(COLOR_PAIR(5));
                    mvprintw(k + 2, j + 3, "%c", '@');
                    attroff(COLOR_PAIR(5));
                }
                else if (main_map.signed_map[j][k] == 'T')
                {
                    attron(COLOR_PAIR(1));
                    mvprintw(k + 2, j + 3, "%s", "\U0001f5dd");
                    attroff(COLOR_PAIR(1));
                }
                else if (main_map.signed_map[j][k] == 'G')
                {
                    attron(COLOR_PAIR(6));
                    mvprintw(k + 2, j + 3, "%c", '@');
                    attroff(COLOR_PAIR(6));
                }
                else if (main_map.signed_map[j][k] == '&')
                {
                    mvprintw(k + 2, j + 3, "%c", '&');
                }
            }
        }
    }
    else if (main_map.rooms[i].room_type == 2)
    {

        for (int j = main_map.rooms[i].start.x; j <= main_map.rooms[i].start.x + main_map.rooms[i].width; ++j)
        {
            for (int k = main_map.rooms[i].start.y; k <= main_map.rooms[i].start.y + main_map.rooms[i].height; ++k)
            {
                if (j == main_map.rooms[i].start.x && k == main_map.rooms[i].start.y)
                {
                    attron(COLOR_PAIR(8));
                    mvprintw(k + 2, j + 3, "%s", "\u250c");
                    attroff(COLOR_PAIR(8));
                }
                else if (j == main_map.rooms[i].start.x && k == main_map.rooms[i].start.y + main_map.rooms[i].height)
                {
                    attron(COLOR_PAIR(8));
                    mvprintw(k + 2, j + 3, "%s", "\u2514");
                    attroff(COLOR_PAIR(8));
                }
                else if (j == main_map.rooms[i].start.x + main_map.rooms[i].width && k == main_map.rooms[i].start.y + main_map.rooms[i].height)
                {
                    attron(COLOR_PAIR(8));
                    mvprintw(k + 2, j + 3, "%s", "\u2518");
                    attroff(COLOR_PAIR(8));
                }
                else if (j == main_map.rooms[i].start.x + main_map.rooms[i].width && k == main_map.rooms[i].start.y)
                {
                    attron(COLOR_PAIR(8));
                    mvprintw(k + 2, j + 3, "%s", "\u2510");
                    attroff(COLOR_PAIR(8));
                }
                else if (main_map.signed_map[j][k] == '-')
                {
                    attron(COLOR_PAIR(8));
                    mvprintw(k + 2, j + 3, "%s", "\u2500");
                    attroff(COLOR_PAIR(8));
                }
                else if (main_map.signed_map[j][k] == '|')
                {
                    attron(COLOR_PAIR(8));
                    mvprintw(k + 2, j + 3, "%s", "\u2502");
                    attroff(COLOR_PAIR(8));
                }
                else if (main_map.signed_map[j][k] == '+' || main_map.signed_map[j][k] == '^' ||
                         main_map.signed_map[j][k] == '<' || main_map.signed_map[j][k] == 'o')
                {
                    attron(COLOR_PAIR(8));
                    mvprintw(k + 2, j + 3, "%c", main_map.signed_map[j][k]);
                    attroff(COLOR_PAIR(8));
                }
                else if (main_map.signed_map[j][k] == 't' || main_map.signed_map[j][k] == '.')
                {
                    mvprintw(k + 2, j + 3, "%s", "\u2022");
                }
                else if (main_map.signed_map[j][k] == 'h')
                {
                    if (main_map.signed_map[j + 1][k] == '-' || main_map.signed_map[j - 1][k] == '-')
                    {
                        attron(COLOR_PAIR(10));
                        mvprintw(k + 2, j + 3, "%s", "\u2500");
                        attroff(COLOR_PAIR(10));
                    }
                    else
                    {
                        attron(COLOR_PAIR(10));
                        mvprintw(k + 2, j + 3, "%s", "\u2502");
                        attroff(COLOR_PAIR(10));
                    }
                }
                else if (main_map.signed_map[j][k] == 'M')
                {
                    attron(COLOR_PAIR(1));
                    mvprintw(k + 2, j + 3, "%s", "\u25B2");
                    attroff(COLOR_PAIR(1));
                }
                else if (main_map.signed_map[j][k] == 'g')
                {
                    attron(COLOR_PAIR(1));
                    mvprintw(k + 2, j + 3, "%s", "\u26c0");
                    attroff(COLOR_PAIR(1));
                }
                else if (main_map.signed_map[j][k] == 'b')
                {
                    attron(COLOR_PAIR(4));
                    mvprintw(k + 2, j + 3, "%s", "\u26c2");
                    attroff(COLOR_PAIR(4));
                }
                else if (main_map.signed_map[j][k] == 'H')
                {
                    attron(COLOR_PAIR(6));
                    mvprintw(k + 2, j + 3, "%s", "\u2695");
                    attroff(COLOR_PAIR(6));
                }
                else if (main_map.signed_map[j][k] == 'S')
                {
                    attron(COLOR_PAIR(8));
                    mvprintw(k + 2, j + 3, "%s", "\u26f7");
                    attroff(COLOR_PAIR(8));
                }
                else if (main_map.signed_map[j][k] == 'D')
                {
                    attron(COLOR_PAIR(4));
                    mvprintw(k + 2, j + 3, "%s", "\u2620");
                    attroff(COLOR_PAIR(4));
                }
                else if (main_map.signed_map[j][k] == 'd' || main_map.signed_map[j][k] == '7')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u2020");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 'w' || main_map.signed_map[j][k] == '8')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u269A");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 'a' || main_map.signed_map[j][k] == '6')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u27B3");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 's')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(k + 2, j + 3, "%s", "\u2694");
                    attroff(COLOR_PAIR(7));
                }
                else if (main_map.signed_map[j][k] == 'f')
                {
                    attron(COLOR_PAIR(9));
                    mvprintw(k + 2, j + 3, "%s", "\u2299");
                    attroff(COLOR_PAIR(9));
                }
                else if (main_map.signed_map[j][k] == 'R')
                {
                    attron(COLOR_PAIR(5));
                    mvprintw(k + 2, j + 3, "%c", '@');
                    attroff(COLOR_PAIR(5));
                }
                else if (main_map.signed_map[j][k] == 'G')
                {
                    attron(COLOR_PAIR(6));
                    mvprintw(k + 2, j + 3, "%c", '@');
                    attroff(COLOR_PAIR(6));
                }
                else if (main_map.signed_map[j][k] == '&')
                {
                    mvprintw(k + 2, j + 3, "%c", '&');
                }
            }
        }
    }
}

void print_game()
{
    // recognize room
    for (int i = 0; i < 92; ++i)
    {
        mvprintw(2, i, "%s", "\u2500");
        mvprintw(0, i, "%s", "\u2500");
    }
    mvprintw(1, 0, "%s", "\u2502");
    mvprintw(1, 91, "%s", "\u2502");
    mvprintw(2, 0, "%s", "\u2514");
    mvprintw(2, 91, "%s", "\u2518");
    mvprintw(0, 0, "%s", "\u250C");
    mvprintw(0, 91, "%s", "\u2510");
    for (int i = 0; i < main_map.num_rooms; ++i)
    {
        if (main_player.showed_room[i] == 1)
        {
            print_room(i);
        }
    }

    for (int i = 0; i < 89; ++i)
    {
        for (int j = 0; j < 30; ++j)
        {
            if (main_player.visited[i][j] == 1 && main_map.signed_map[i][j] == '#')
            {
                mvprintw(j + 2, i + 3, "%c", main_map.signed_map[i][j]);
            }
            else if (main_map.signed_map[i][j] == '7')
            {
                attron(COLOR_PAIR(7));
                mvprintw(j + 2, i + 3, "%s", "\u2020");
                attroff(COLOR_PAIR(7));
            }
            else if (main_map.signed_map[i][j] == '8')
            {
                attron(COLOR_PAIR(7));
                mvprintw(j + 2, i + 3, "%s", "\u269A");
                attroff(COLOR_PAIR(7));
            }
            else if (main_map.signed_map[i][j] == '6')
            {
                attron(COLOR_PAIR(7));
                mvprintw(j + 2, i + 3, "%s", "\u27B3");
                attroff(COLOR_PAIR(7));
            }
        }
    }

    int dx[4] = {1, 0, -1, 0};
    int dy[4] = {0, 1, 0, -1};

    if (main_map.signed_map[main_player.position.x][main_player.position.y] == '#')
    {
        for (int i = 0; i < 4; ++i)
        {
            if (main_map.signed_map[main_player.position.x + dx[i]][main_player.position.y + dy[i]] == '+' ||
                main_map.signed_map[main_player.position.x + dx[i]][main_player.position.y + dy[i]] == 'R' ||
                main_map.signed_map[main_player.position.x + dx[i]][main_player.position.y + dy[i]] == 'G' ||
                main_map.signed_map[main_player.position.x + dx[i]][main_player.position.y + dy[i]] == 'h')
            {
                point p;
                p.x = main_player.position.x + dx[i];
                p.y = main_player.position.y + dy[i];
                int room_index = recognize_rooms(p);
                print_room(room_index);
                break;
            }
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        if (main_map.signed_map[main_player.position.x + dx[i]][main_player.position.y + dy[i]] == '#')
        {
            mvprintw(main_player.position.y + dy[i] + 2, main_player.position.x + dx[i] + 3, "%c", '#');
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        if (main_map.signed_map[main_player.position.x + dx[i]][main_player.position.y + dy[i]] == '#')
        {
            if (i == 0)
            {
                for (int j = main_player.position.x; j >= main_player.position.x - 5; --j)
                {
                    if ((0 < j) && (89 > j) && (main_map.signed_map[j][main_player.position.y] == '#') &&
                        (main_map.signed_map[main_player.position.x][main_player.position.y] == '#'))
                    {
                        if (main_map.signed_map[j + 1][main_player.position.y] == '#')
                        {
                            mvprintw(main_player.position.y + 2, j + 3, "%c", '#');
                        }
                    }
                }
            }
            else if (i == 1)
            {
                for (int j = main_player.position.y; j >= main_player.position.y - 5; --j)
                {
                    if ((0 < j) && (89 > j) && (main_map.signed_map[main_player.position.x][j] == '#') &&
                        (main_map.signed_map[main_player.position.x][main_player.position.y] == '#'))
                    {
                        if (main_map.signed_map[main_player.position.x][j + 1] == '#')
                        {
                            mvprintw(j + 2, main_player.position.x + 3, "%c", '#');
                        }
                    }
                }
            }
            else if (i == 2)
            {
                for (int j = main_player.position.x; j <= main_player.position.x + 5; ++j)
                {
                    if ((0 < j) && (89 > j) && (main_map.signed_map[j][main_player.position.y] == '#') &&
                        (main_map.signed_map[main_player.position.x][main_player.position.y] == '#'))
                    {
                        if (main_map.signed_map[j - 1][main_player.position.y] == '#')
                        {
                            mvprintw(main_player.position.y + 2, j + 3, "%c", '#');
                        }
                    }
                }
            }
            else if (i == 3)
            {
                for (int j = main_player.position.y; j <= main_player.position.y + 5; ++j)
                {
                    if ((0 < j) && (89 > j) && (main_map.signed_map[main_player.position.x][j] == '#') &&
                        (main_map.signed_map[main_player.position.x][main_player.position.y] == '#'))
                    {
                        if (main_map.signed_map[main_player.position.x][j - 1] == '#')
                        {
                            mvprintw(j + 2, main_player.position.x + 3, "%c", '#');
                        }
                    }
                }
            }
        }
    }

    // messeges
    attron(COLOR_PAIR(1));
    mvprintw(1, 83, "Gold:%d", main_player.usual_golds + main_player.black_golds * 5);
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(6));
    int showing_health = (main_player.health * 100 / main_player.initial_health != 0) ? main_player.health * 100 / main_player.initial_health : 1;
    mvprintw(1, 70, "Health:%d", showing_health);
    printw("%c", '%');
    attroff(COLOR_PAIR(6));
    time_t current_time = time(NULL);
    if (difftime(current_time, main_player.last_time) > 5.0)
    {
        main_player.hunger += 3;

        if (main_player.hunger >= main_player.initial_hunger / 2)
        {
            if (main_player.is_health_enchant_active == 0)
            {
                if (main_player.health >= 1)
                    main_player.health -= 1;
            }
            else
            {
                if (main_player.hunger >= 3)
                    main_player.hunger -= 3;
            }
        }

        main_player.last_time = time(NULL);
    }

    if ((difftime(current_time, main_player.last_attack) > 6.0) && (main_player.hunger == 0))
    {
        if (main_player.is_health_enchant_active == 1)
            main_player.health = (main_player.health >= main_player.initial_health - 5) ? main_player.health : main_player.health + 5;
        else
        {
            main_player.health = (main_player.health >= main_player.initial_health - 2) ? main_player.health : main_player.health + 2;
        }
    }

    if (main_player.has_password == 1 && difftime(current_time, main_player.last_password) < 10.0)
    {
        attron(COLOR_PAIR(6));
        mvprintw(1, 1, "%s", "             ");
        mvprintw(1, 1, "Password:%d", main_player.showed_password);
        attroff(COLOR_PAIR(6));
    }

    if (main_player.is_health_enchant_active == 1)
    {
        main_player.health = (main_player.health >= main_player.initial_health - 2) ? main_player.initial_health : main_player.health + 2;
    }

    for (int i = 0; i < main_map.num_enemies; ++i)
    {
        if (enemies[i].health > 0)
        {
            if (enemies[i].num_room == recognize_rooms(main_player.position) || main_player.showed_room[enemies[i].num_room])
            {
                if (enemies[i].type == 1)
                {
                    attron(COLOR_PAIR(5));
                    mvprintw(enemies[i].position.y + 2, enemies[i].position.x + 3, "%s", "\U0001d507");
                    attroff(COLOR_PAIR(5));
                }
                else if (enemies[i].type == 2)
                {
                    attron(COLOR_PAIR(5));
                    mvprintw(enemies[i].position.y + 2, enemies[i].position.x + 3, "%s", "\U0001d509");
                    attroff(COLOR_PAIR(5));
                }
                else if (enemies[i].type == 3)
                {
                    attron(COLOR_PAIR(5));
                    mvprintw(enemies[i].position.y + 2, enemies[i].position.x + 3, "%s", "\U0001D5A6");
                    attroff(COLOR_PAIR(5));
                }
                else if (enemies[i].type == 4)
                {
                    attron(COLOR_PAIR(5));
                    mvprintw(enemies[i].position.y + 2, enemies[i].position.x + 3, "%s", "\U0001d516");
                    attroff(COLOR_PAIR(5));
                }
                else if (enemies[i].type == 5)
                {
                    attron(COLOR_PAIR(5));
                    mvprintw(enemies[i].position.y + 2, enemies[i].position.x + 3, "%s", "\U0001d518");
                    attroff(COLOR_PAIR(5));
                }
            }
        }
    }

    if (GENERAL_SETTING.COLOR == 1)
    {
        attron(COLOR_PAIR(5));
        mvprintw(main_player.position.y + 2, main_player.position.x + 3, "%s", "\u263B");
        attroff(COLOR_PAIR(5));
    }
    else if (GENERAL_SETTING.COLOR == 2)
    {
        attron(COLOR_PAIR(7));
        mvprintw(main_player.position.y + 2, main_player.position.x + 3, "%s", "\u263B");
        attroff(COLOR_PAIR(7));
    }
    else if (GENERAL_SETTING.COLOR == 3)
    {
        attron(COLOR_PAIR(8));
        mvprintw(main_player.position.y + 2, main_player.position.x + 3, "%s", "\u263B");
        attroff(COLOR_PAIR(8));
    }
    else if (GENERAL_SETTING.COLOR == 4)
    {
        attron(COLOR_PAIR(6));
        mvprintw(main_player.position.y + 2, main_player.position.x + 3, "%s", "\u263B");
        attroff(COLOR_PAIR(6));
    }
    else
    {
        mvprintw(main_player.position.y + 2, main_player.position.x + 3, "%s", "\u263B");
    }

    move(main_player.position.y + 2, main_player.position.x + 3);
    refresh();
}

void next_floor()
{
    if (main_map.signed_map[main_player.position.x][main_player.position.y] == '<')
    {
        main_player.num_wrong_password = 0;
        main_player.password = 1;
        main_player.has_password = 0;
        main_player.showed_password = 1;
        for (int i = 0; i < 92; ++i)
        {
            for (int j = 0; j < 32; ++j)
            {
                main_player.visited[i][j] = 0;
            }
        }
        main_map.floor += 1;
        enemies = NULL;
        create_map();
        initialize_enemies();

        for (int i = 0; i < 7; ++i)
        {
            main_player.showed_room[i] = 0;
        }

        initialize_random_position(&main_player);
        clear();
        attron(COLOR_PAIR(6));
        mvprintw(1, 1, "%s", "You entered the next floor");
        attroff(COLOR_PAIR(6));
        print_game();
        refresh();
    }
}

int end_game()
{
    if (main_map.floor == 5)
    {
        int total_health = 0;
        for (int i = 0; i < main_map.num_enemies; ++i)
        {
            if (enemies[i].health > 0)
                total_health += enemies[i].health;
        }

        if (total_health <= 0)
        {
            // save score and gold

            if (strcmp(GENERAL_SETTING.USERNAME, "Guest") != 0)
            {

                int index = 0;
                for (int i = 0; i < main_scoreboard.num_users; ++i)
                {
                    if (strcmp(GENERAL_SETTING.USERNAME, main_scoreboard.usernames[i]) == 0)
                    {
                        index = i;
                        break;
                    }
                }

                main_scoreboard.golds[index] += (main_player.black_golds * 5 + main_player.usual_golds);
                main_scoreboard.score[index] += ((main_player.black_golds * 5 + main_player.usual_golds) * 10 +
                                                 (main_player.health * 10) / main_player.initial_health - (main_player.hunger * 10) / main_player.initial_hunger + 10) +
                                                100;

                FILE *scoreboard_file = fopen("scoreboard.bin", "wb");
                fwrite(&main_scoreboard, sizeof(users_scoreboard), 1, scoreboard_file);
                fclose(scoreboard_file);

                char *map_save = (char *)malloc(50 * sizeof(char));
                char *player_save = (char *)malloc(50 * sizeof(char));
                char *general_setting_save = (char *)malloc(50 * sizeof(char));
                char *enemies_save = (char *)malloc(50 * sizeof(char));

                sprintf(map_save, "%s/map.bin", GENERAL_SETTING.USERNAME);
                sprintf(player_save, "%s/player.bin", GENERAL_SETTING.USERNAME);
                sprintf(general_setting_save, "%s/generalsetting.bin", GENERAL_SETTING.USERNAME);
                sprintf(enemies_save, "%s/enemies.bin", GENERAL_SETTING.USERNAME);

                remove(map_save);
                remove(player_save);
                remove(general_setting_save);
                remove(enemies_save);

                free(map_save);
                free(player_save);
                free(general_setting_save);
                free(enemies_save);
            }
            // save score and gold

            int score = ((main_player.black_golds * 5 + main_player.usual_golds) * 10 +
                         (main_player.health * 10) / main_player.initial_health - (main_player.hunger * 10) / main_player.initial_hunger + 10) +
                        100;

            clear();
            flushinp();
            noecho();
            keypad(stdscr, TRUE);
            nodelay(stdscr, TRUE);
            int ch = 100;
            mvprintw(30, 32, "%s", "Press any key to return to meun");
            flushinp();
            while (ch != 10)
            {
                show_victory(score);
                refresh();
                ch = getch();
                if (ch != ERR)
                {
                    break;
                }
            }
            nodelay(stdscr, FALSE);
            return -1;
        }
    }

    if ((main_player.health <= 0) || (main_player.hunger >= main_player.initial_hunger))
    {
        // save score and gold and delte last game
        if (strcmp(GENERAL_SETTING.USERNAME, "Guest") != 0)
        {

            int index = 0;
            for (int i = 0; i < main_scoreboard.num_users; ++i)
            {
                if (strcmp(GENERAL_SETTING.USERNAME, main_scoreboard.usernames[i]) == 0)
                {
                    index = i;
                    break;
                }
            }

            main_scoreboard.golds[index] += (main_player.black_golds * 5 + main_player.usual_golds);
            main_scoreboard.score[index] += ((main_player.black_golds * 5 + main_player.usual_golds) * 10 +
                                             (main_player.health * 10) / main_player.initial_health - (main_player.hunger * 10) / main_player.initial_hunger + 10);

            FILE *scoreboard_file = fopen("scoreboard.bin", "wb");
            fwrite(&main_scoreboard, sizeof(users_scoreboard), 1, scoreboard_file);
            fclose(scoreboard_file);

            char *map_save = (char *)malloc(50 * sizeof(char));
            char *player_save = (char *)malloc(50 * sizeof(char));
            char *general_setting_save = (char *)malloc(50 * sizeof(char));
            char *enemies_save = (char *)malloc(50 * sizeof(char));

            sprintf(map_save, "%s/map.bin", GENERAL_SETTING.USERNAME);
            sprintf(player_save, "%s/player.bin", GENERAL_SETTING.USERNAME);
            sprintf(general_setting_save, "%s/generalsetting.bin", GENERAL_SETTING.USERNAME);
            sprintf(enemies_save, "%s/enemies.bin", GENERAL_SETTING.USERNAME);

            remove(map_save);
            remove(player_save);
            remove(general_setting_save);
            remove(enemies_save);

            free(map_save);
            free(player_save);
            free(general_setting_save);
            free(enemies_save);
        }
        // save score and gold and delete last game
        int score = ((main_player.black_golds * 5 + main_player.usual_golds) * 10 +
                     (main_player.health * 10) / main_player.initial_health - (main_player.hunger * 10) / main_player.initial_hunger + 10);
        clear();

        flushinp();
        noecho();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        int ch = 100;
        mvprintw(30, 32, "%s", "Press any key to return to meun");
        flushinp();
        while (ch != 10)
        {
            show_tombstone(score);
            refresh();
            ch = getch();
            if (ch != ERR)
            {
                break;
            }
        }
        nodelay(stdscr, FALSE);
        return -1;
    }
}

int pause_menu()
{
    clear();
    char *options[] = {"Save", "Resume", "Exit"};
    int option_numbers = 3;
    int highlight = 0;
    int ch;
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    while (1)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (highlight == i)
            {
                attron(A_REVERSE);
                attron(A_BOLD);
                mvprintw(15 + 2 * (i - 1), 40, "%s", options[i]);
                attroff(A_BOLD);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(15 + 2 * (i - 1), 40, "%s", options[i]);
            }
        }
        refresh();
        ch = getch();

        if (ch == KEY_DOWN)
        {
            if (highlight < 2)
                ++highlight;
        }
        else if (ch == KEY_UP)
        {
            if (highlight > 0)
                --highlight;
        }
        else if (ch == 10)
        {
            if (highlight == 0)
            {
                if (strcmp(GENERAL_SETTING.USERNAME, "Guest") == 0)
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 33, "%s", "Can't save while you're Guest!");
                    attroff(COLOR_PAIR(5));
                    refresh();
                    napms(2000);
                    return 3;
                }
                else
                {
                    clear();
                    noecho();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    return 1;
                }
            }
            else if (highlight == 1)
            {
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
                return 2;
            }
            else if (highlight == 2)
            {
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
                return 0;
            }
        }
    }

    // 0 --> exit
    // 1 --> save
    // else --> continue
}

int save_game()
{

    GENERAL_SETTING.num_enemies = main_map.num_enemies;
    GENERAL_SETTING.num_rooms = main_map.num_rooms;

    enemy saving_enemies[GENERAL_SETTING.num_enemies];

    for (int i = 0; i < GENERAL_SETTING.num_enemies; ++i)
    {
        saving_enemies[i] = enemies[i];
    }

    saving_map copy_map;
    copy_map.floor = main_map.floor;
    copy_map.master_key.x = main_map.master_key.x;
    copy_map.master_key.y = main_map.master_key.y;
    copy_map.num_enemies = main_map.num_enemies;
    copy_map.num_rooms = main_map.num_rooms;
    copy_map.stairs.x = main_map.stairs.x;
    copy_map.stairs.y = main_map.stairs.y;

    for (int i = 0; i < 92; ++i)
    {
        for (int j = 0; j < 32; ++j)
        {
            copy_map.signed_map[i][j] = main_map.signed_map[i][j];
        }
    }

    for (int i = 0; i < 7; ++i)
    {
        copy_map.rooms[i].start.x = main_map.rooms[i].start.x;
        copy_map.rooms[i].start.y = main_map.rooms[i].start.y;
        copy_map.rooms[i].width = main_map.rooms[i].width;
        copy_map.rooms[i].height = main_map.rooms[i].height;
        copy_map.rooms[i].num_doors = main_map.rooms[i].num_doors;
        copy_map.rooms[i].num_enemies = main_map.rooms[i].num_enemies;
        copy_map.rooms[i].room_type = main_map.rooms[i].room_type;
    }

    char *map_save = (char *)malloc(50 * sizeof(char));
    char *player_save = (char *)malloc(50 * sizeof(char));
    char *general_setting_save = (char *)malloc(50 * sizeof(char));
    char *enemies_save = (char *)malloc(50 * sizeof(char));

    sprintf(map_save, "%s/map.bin", GENERAL_SETTING.USERNAME);
    sprintf(player_save, "%s/player.bin", GENERAL_SETTING.USERNAME);
    sprintf(general_setting_save, "%s/generalsetting.bin", GENERAL_SETTING.USERNAME);
    sprintf(enemies_save, "%s/enemies.bin", GENERAL_SETTING.USERNAME);

    FILE *file_1 = fopen(map_save, "wb");
    fwrite(&copy_map, sizeof(saving_map), 1, file_1);
    fclose(file_1);

    FILE *file_2 = fopen(player_save, "wb");
    fwrite(&main_player, sizeof(player), 1, file_2);
    fclose(file_2);

    FILE *file_3 = fopen(general_setting_save, "wb");
    fwrite(&GENERAL_SETTING, sizeof(general_setting), 1, file_3);
    fclose(file_3);

    FILE *file_4 = fopen(enemies_save, "wb");
    fwrite(&saving_enemies, sizeof(enemy), GENERAL_SETTING.num_enemies, file_4);
    fclose(file_4);

    free(general_setting_save);
    free(map_save);
    free(player_save);
    free(enemies_save);

    return 1;
}

void consume_food()
{
    clear();
    attron(COLOR_PAIR(8));
    mvprintw(11, 30, "Hunger:%d from 100", main_player.hunger * 100 / 100);
    mvprintw(13, 30, "Number of foods in your backpack:%d", main_player.number_of_foods);
    attroff(COLOR_PAIR(8));

    char *options[] = {"Consume food", "Resume"};
    int option_numbers = 2;
    int highlight = 0;
    int ch;
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    while (1)
    {
        for (int i = 0; i < 2; ++i)
        {
            if (highlight == i)
            {
                attron(A_REVERSE);
                attron(A_BOLD);
                attron(COLOR_PAIR(8));
                mvprintw(17 + 2 * i, 30, "%s", options[i]);
                attroff(COLOR_PAIR(8));
                attroff(A_BOLD);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(17 + 2 * i, 30, "%s", options[i]);
            }
        }
        refresh();
        ch = getch();

        if (ch == KEY_DOWN)
        {
            if (highlight < 1)
                ++highlight;
        }
        else if (ch == KEY_UP)
        {
            if (highlight > 0)
                --highlight;
        }
        else if (ch == 10)
        {
            if (highlight == 0)
            {
                if (main_player.number_of_foods > 0)
                {
                    --main_player.number_of_foods;
                    clear();
                    attron(COLOR_PAIR(6));
                    mvprintw(16, 37, "%s", "You consumed a food!");
                    attroff(COLOR_PAIR(6));
                    refresh();
                    napms(1500);
                    clear();
                    noecho();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    return;
                }
                else
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 33, "%s", "You don't have enough food!");
                    attroff(COLOR_PAIR(5));
                    refresh();
                    napms(1500);
                    clear();
                    noecho();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    return;
                }
            }
            else if (highlight == 1)
            {
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
                return;
            }
        }
    }
}

void search_around()
{
    // khodesh napare
    // traps or hidden doors
    for (int i = main_player.position.x - 1; i < main_player.position.x + 2; ++i)
    {
        for (int j = main_player.position.y - 1; j < main_player.position.y + 2; ++j)
        {
            if ((i != main_player.position.x) || (j != main_player.position.y))
            {
                if (main_map.signed_map[i][j] == 'h' || main_map.signed_map[i][j] == 't')
                {
                    mvprintw(j + 2, i + 3, "%c", main_map.signed_map[i][j]);
                }
            }
        }
    }
    move(main_player.position.y + 2, main_player.position.x + 3);
    refresh();
    napms(700);
}

void view_whole_map()
{

    clear();

    for (int y = 0; y < 30; ++y)
    {
        for (int x = 0; x < 89; ++x)
        {
            if (main_map.signed_map[x][y] == '.')
            {
                mvprintw(y + 2, x + 3, "%s", "\u2022");
            }
            else if (main_map.signed_map[x][y] == 'M')
            {
                attron(COLOR_PAIR(1));
                mvprintw(y + 2, x + 3, "%s", "\u25B2");
                attroff(COLOR_PAIR(1));
            }
            else if (main_map.signed_map[x][y] == 'g')
            {
                attron(COLOR_PAIR(1));
                mvprintw(y + 2, x + 3, "%s", "\u26c0");
                attroff(COLOR_PAIR(1));
            }
            else if (main_map.signed_map[x][y] == 'b')
            {
                attron(COLOR_PAIR(4));
                mvprintw(y + 2, x + 3, "%s", "\u26c2");
                attroff(COLOR_PAIR(4));
            }
            else if (main_map.signed_map[x][y] == 'H')
            {
                attron(COLOR_PAIR(6));
                mvprintw(y + 2, x + 3, "%s", "\u2695");
                attroff(COLOR_PAIR(6));
            }
            else if (main_map.signed_map[x][y] == 'S')
            {
                attron(COLOR_PAIR(8));
                mvprintw(y + 2, x + 3, "%s", "\u26f7");
                attroff(COLOR_PAIR(8));
            }
            else if (main_map.signed_map[x][y] == 'D')
            {
                attron(COLOR_PAIR(4));
                mvprintw(y + 2, x + 3, "%s", "\u2620");
                attroff(COLOR_PAIR(4));
            }
            else if (main_map.signed_map[x][y] == 'd' || main_map.signed_map[x][y] == '7')
            {
                attron(COLOR_PAIR(7));
                mvprintw(y + 2, x + 3, "%s", "\u2020");
                attroff(COLOR_PAIR(7));
            }
            else if (main_map.signed_map[x][y] == 'w' || main_map.signed_map[x][y] == '8')
            {
                attron(COLOR_PAIR(7));
                mvprintw(y + 2, x + 3, "%s", "\u269A");
                attroff(COLOR_PAIR(7));
            }
            else if (main_map.signed_map[x][y] == 'a' || main_map.signed_map[x][y] == '6')
            {
                attron(COLOR_PAIR(7));
                mvprintw(y + 2, x + 3, "%s", "\u27B3");
                attroff(COLOR_PAIR(7));
            }
            else if (main_map.signed_map[x][y] == '-' || main_map.signed_map[x][y] == '|')
            {
            }
            else if (main_map.signed_map[x][y] == 's')
            {
                attron(COLOR_PAIR(7));
                mvprintw(y + 2, x + 3, "%s", "\u2694");
                attroff(COLOR_PAIR(7));
            }
            else if (main_map.signed_map[x][y] == 'f')
            {
                attron(COLOR_PAIR(9));
                mvprintw(y + 2, x + 3, "%s", "\u2299");
                attroff(COLOR_PAIR(9));
            }
            else if (main_map.signed_map[x][y] == 'h')
            {
                mvprintw(y + 2, x + 3, "%s", "\u2022");
            }
            else if (main_map.signed_map[x][y] == 't')
            {
                mvprintw(y + 2, x + 3, "%s", "\u2022");
            }
            else if (main_map.signed_map[x][y] == 'R')
            {
                attron(COLOR_PAIR(5));
                mvprintw(y + 2, x + 3, "%c", '@');
                attroff(COLOR_PAIR(5));
            }
            else if (main_map.signed_map[x][y] == 'T')
            {
                attron(COLOR_PAIR(1));
                mvprintw(y + 2, x + 3, "%s", "\U0001f5dd");
                attroff(COLOR_PAIR(1));
            }
            else if (main_map.signed_map[x][y] == 'G')
            {
                attron(COLOR_PAIR(6));
                mvprintw(y + 2, x + 3, "%c", '@');
                attroff(COLOR_PAIR(6));
            }
            else
            {
                mvprintw(y + 2, x + 3, "%c", main_map.signed_map[x][y]);
            }
        }
        printw("\n");
    }

    print_wall();

    for (int i = 0; i < main_map.num_enemies; ++i)
    {
        if (enemies[i].health > 0)
        {
            if (enemies[i].type == 1)
            {
                attron(COLOR_PAIR(5));
                mvprintw(enemies[i].position.y + 2, enemies[i].position.x + 3, "%s", "\U0001d507");
                attroff(COLOR_PAIR(5));
            }
            else if (enemies[i].type == 2)
            {
                attron(COLOR_PAIR(5));
                mvprintw(enemies[i].position.y + 2, enemies[i].position.x + 3, "%s", "\U0001d509");
                attroff(COLOR_PAIR(5));
            }
            else if (enemies[i].type == 3)
            {
                attron(COLOR_PAIR(5));
                mvprintw(enemies[i].position.y + 2, enemies[i].position.x + 3, "%s", "\U0001D5A6");
                attroff(COLOR_PAIR(5));
            }
            else if (enemies[i].type == 4)
            {
                attron(COLOR_PAIR(5));
                mvprintw(enemies[i].position.y + 2, enemies[i].position.x + 3, "%s", "\U0001d516");
                attroff(COLOR_PAIR(5));
            }
            else if (enemies[i].type == 5)
            {
                attron(COLOR_PAIR(5));
                mvprintw(enemies[i].position.y + 2, enemies[i].position.x + 3, "%s", "\U0001d518");
                attroff(COLOR_PAIR(5));
            }
        }
    }

    if (GENERAL_SETTING.COLOR == 1)
    {
        attron(COLOR_PAIR(5));
        mvprintw(main_player.position.y + 2, main_player.position.x + 3, "%s", "\u263B");
        attroff(COLOR_PAIR(5));
    }
    else if (GENERAL_SETTING.COLOR == 2)
    {
        attron(COLOR_PAIR(7));
        mvprintw(main_player.position.y + 2, main_player.position.x + 3, "%s", "\u263B");
        attroff(COLOR_PAIR(7));
    }
    else if (GENERAL_SETTING.COLOR == 3)
    {
        attron(COLOR_PAIR(8));
        mvprintw(main_player.position.y + 2, main_player.position.x + 3, "%s", "\u263B");
        attroff(COLOR_PAIR(8));
    }
    else if (GENERAL_SETTING.COLOR == 4)
    {
        attron(COLOR_PAIR(6));
        mvprintw(main_player.position.y + 2, main_player.position.x + 3, "%s", "\u263B");
        attroff(COLOR_PAIR(6));
    }
    else
    {
        mvprintw(main_player.position.y + 2, main_player.position.x + 3, "%s", "\u263B");
    }

    move(main_player.position.y + 2, main_player.position.x + 3);
    refresh();
    napms(3000);
    clear();
}

void list_of_enchant()
{
    clear();

    attron(COLOR_PAIR(8));
    mvprintw(7, 30, "%s", "Enchant");
    mvprintw(7, 45, "%s", "Number");
    attroff(COLOR_PAIR(8));

    attron(COLOR_PAIR(5));
    mvprintw(10, 30, "%s", "Health");
    mvprintw(12, 30, "%s", "Speed");
    mvprintw(14, 30, "%s", "Damage");
    attroff(COLOR_PAIR(5));

    for (int i = 0; i < main_player.number_of_health_enchant; ++i)
    {
        attron(COLOR_PAIR(6));
        mvprintw(10, 45 + i, "%s", "\u2695");
        attroff(COLOR_PAIR(6));
    }

    for (int i = 0; i < main_player.number_of_speed_enchant; ++i)
    {
        attron(COLOR_PAIR(8));
        mvprintw(12, 45 + i, "%s", "\u26f7");
        attroff(COLOR_PAIR(8));
    }

    for (int i = 0; i < main_player.number_of_damage_enchant; ++i)
    {
        attron(COLOR_PAIR(4));
        mvprintw(14, 45 + i, "%s", "\u2620");
        attroff(COLOR_PAIR(4));
    }

    char *options[] = {"Consume Health enchant", "Consume Speed enchant", "Consume Damage enchant", "Resume"};
    int option_numbers = 4;
    int highlight = 0;
    int ch;
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    while (1)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (highlight == i)
            {
                attron(A_REVERSE);
                attron(A_BOLD);
                attron(COLOR_PAIR(8));
                mvprintw(17 + 2 * i, 30, "%s", options[i]);
                attroff(COLOR_PAIR(8));
                attroff(A_BOLD);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(17 + 2 * i, 30, "%s", options[i]);
            }
        }
        refresh();
        ch = getch();

        if (ch == KEY_DOWN)
        {
            if (highlight < 3)
                ++highlight;
        }
        else if (ch == KEY_UP)
        {
            if (highlight > 0)
                --highlight;
        }
        else if (ch == 10)
        {
            if (highlight == 0)
            {
                if (main_player.number_of_health_enchant > 0)
                {
                    --main_player.number_of_health_enchant;
                    main_player.is_health_enchant_active = 1;
                    main_player.health_enchant_counter = 10;
                    clear();
                    attron(COLOR_PAIR(6));
                    mvprintw(16, 33, "%s", "You consumed a Health enchant!");
                    attroff(COLOR_PAIR(6));
                    refresh();
                    napms(1500);
                    clear();
                    noecho();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    return;
                }
                else
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 33, "%s", "You don't have enough Health enchant!");
                    attroff(COLOR_PAIR(5));
                    refresh();
                    napms(1500);
                    clear();
                    noecho();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    return;
                }
            }
            else if (highlight == 1)
            {
                if (main_player.number_of_speed_enchant > 0)
                {
                    --main_player.number_of_speed_enchant;
                    main_player.is_speed_enchant_active = 1;
                    main_player.speed_enchant_counter = 20;
                    clear();
                    attron(COLOR_PAIR(6));
                    mvprintw(16, 33, "%s", "You consumed a Speed enchant!");
                    attroff(COLOR_PAIR(6));
                    refresh();
                    napms(1500);
                    clear();
                    noecho();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    return;
                }
                else
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 33, "%s", "You don't have enough Speed enchant!");
                    attroff(COLOR_PAIR(5));
                    refresh();
                    napms(1500);
                    clear();
                    noecho();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    return;
                }
            }
            else if (highlight == 2)
            {
                if (main_player.number_of_damage_enchant > 0)
                {
                    --main_player.number_of_damage_enchant;
                    main_player.is_damage_enchant_active = 1;
                    main_player.damage_enchant_counter = 10;
                    clear();
                    attron(COLOR_PAIR(6));
                    mvprintw(16, 33, "%s", "You consumed a Damage enchant!");
                    attroff(COLOR_PAIR(6));
                    refresh();
                    napms(1500);
                    clear();
                    noecho();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    return;
                }
                else
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 33, "%s", "You don't have enough Damage enchant!");
                    attroff(COLOR_PAIR(5));
                    refresh();
                    napms(1500);
                    clear();
                    noecho();
                    keypad(stdscr, TRUE);
                    curs_set(0);
                    return;
                }
            }
            else if (highlight == 3)
            {
                return;
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
            }
        }
    }
}

void list_of_ammos()
{
    /*
  1 --> mace
  2  --> arrow
  3 --> sword
  4 --> wand
  5 --> dagger
  0 --> nothing
  */

    clear();
    attron(COLOR_PAIR(8));
    mvprintw(3, 20, "%s", "Ammo");
    mvprintw(3, 30, "%s", "Number");
    mvprintw(3, 50, "%s", "Power");
    mvprintw(3, 70, "%s", "Range");
    attroff(COLOR_PAIR(8));

    attron(COLOR_PAIR(5));
    if (main_player.default_ammo == 1)
    {
        attron(A_BOLD);
        mvprintw(6, 20, "%s", "Mace");
        attroff(A_BOLD);
    }
    else
    {
        mvprintw(6, 20, "%s", "Mace");
    }

    if (main_player.default_ammo == 2)
    {
        attron(A_BOLD);
        mvprintw(8, 20, "%s", "Arrow");
        attroff(A_BOLD);
    }
    else
    {
        mvprintw(8, 20, "%s", "Arrow");
    }

    if (main_player.default_ammo == 3)
    {
        attron(A_BOLD);
        mvprintw(10, 20, "%s", "Sword");
        attroff(A_BOLD);
    }
    else
    {
        mvprintw(10, 20, "%s", "Sword");
    }

    if (main_player.default_ammo == 4)
    {
        attron(A_BOLD);
        mvprintw(12, 20, "%s", "Wand");
        attroff(A_BOLD);
    }
    else
    {
        mvprintw(12, 20, "%s", "Wand");
    }

    if (main_player.default_ammo == 5)
    {
        attron(A_BOLD);
        mvprintw(14, 20, "%s", "Dagger");
        attroff(A_BOLD);
    }
    else
    {
        mvprintw(14, 20, "%s", "Dagger");
    }
    attroff(COLOR_PAIR(5));

    attron(COLOR_PAIR(6));
    for (int i = 0; i < 5; ++i)
    {
        mvprintw(6, 50 + i, "%s", "\u25Ac");
        mvprintw(8, 50 + i, "%s", "\u25Ac");
    }

    for (int i = 0; i < 10; ++i)
    {
        mvprintw(10, 50 + i, "%s", "\u25Ac");
    }

    for (int i = 0; i < 12; ++i)
    {
        mvprintw(14, 50 + i, "%s", "\u25Ac");
    }

    for (int i = 0; i < 15; ++i)
    {
        mvprintw(12, 50 + i, "%s", "\u25Ac");
    }

    for (int i = 0; i < 1; ++i)
    {
        mvprintw(6, 70 + i, "%s", "\u25Ac");
        mvprintw(10, 70 + i, "%s", "\u25Ac");
    }

    for (int i = 0; i < 5; ++i)
    {
        mvprintw(8, 70 + i, "%s", "\u25Ac");
        mvprintw(14, 70 + i, "%s", "\u25Ac");
    }

    for (int i = 0; i < 10; ++i)
    {
        mvprintw(12, 70 + i, "%s", "\u25Ac");
    }

    mvprintw(6, 30, "%s", "\u25Ac");
    printw("%s", "   x1");

    int arrow_num_scale = (main_player.number_of_arrow % 10) ? main_player.number_of_arrow / 10 + 1 : main_player.number_of_arrow / 10;
    for (int i = 0; i < arrow_num_scale; ++i)
    {
        mvprintw(8, 30 + i, "%s", "\u25Ac");
    }
    if (arrow_num_scale)
        mvprintw(8, 30 + arrow_num_scale, "%s%d", "   x", main_player.number_of_arrow);

    int sword_num_scale = (main_player.number_of_sword % 10) ? main_player.number_of_sword / 10 + 1 : main_player.number_of_sword / 10;
    for (int i = 0; i < sword_num_scale; ++i)
    {
        mvprintw(10, 30 + i, "%s", "\u25Ac");
    }
    if (sword_num_scale)
        mvprintw(10, 30 + sword_num_scale, "%s%d", "   x", main_player.number_of_sword);

    int wand_num_scale = (main_player.number_of_wand % 10) ? main_player.number_of_wand / 10 + 1 : main_player.number_of_wand / 10;
    for (int i = 0; i < wand_num_scale; ++i)
    {
        mvprintw(12, 30 + i, "%s", "\u25Ac");
    }
    if (wand_num_scale)
        mvprintw(12, 30 + wand_num_scale, "%s%d", "   x", main_player.number_of_wand);

    int dagger_num_scale = (main_player.number_of_dagger % 10) ? main_player.number_of_dagger / 10 + 1 : main_player.number_of_dagger / 10;
    for (int i = 0; i < dagger_num_scale; ++i)
    {
        mvprintw(14, 30 + i, "%s", "\u25Ac");
    }
    if (dagger_num_scale)
        mvprintw(14, 30 + dagger_num_scale, "%s%d", "   x", main_player.number_of_dagger);

    attroff(COLOR_PAIR(6));

    attron(COLOR_PAIR(8));
    mvprintw(17, 20, "%s", "Change default ammo:");
    mvprintw(17, 50, "%s", "Short range");
    mvprintw(17, 70, "%s", "Short range");
    for (int i = 0; i < 5; ++i)
    {
        mvprintw(19, 50 + i, "%s", "\u25Ac");
        mvprintw(21, 70 + i, "%s", "\u25Ac");
        mvprintw(23, 50 + i, "%s", "\u25Ac");
        mvprintw(25, 70 + i, "%s", "\u25Ac");
        mvprintw(27, 70 + i, "%s", "\u25Ac");
    }
    attroff(COLOR_PAIR(8));

    char *options[] = {"Mace", "Arrow", "Sword", "Wand", "Dagger", "Resume"};
    int option_numbers = 6;
    int highlight = main_player.default_ammo - 1;
    int ch;
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    while (1)
    {
        for (int i = 0; i < 6; ++i)
        {
            if (highlight == i)
            {
                attron(A_REVERSE);
                attron(A_BOLD);
                attron(COLOR_PAIR(8));
                mvprintw(19 + 2 * i, 20, "%s", options[i]);
                attroff(COLOR_PAIR(8));
                attroff(A_BOLD);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(19 + 2 * i, 20, "%s", options[i]);
            }
        }
        refresh();
        ch = getch();

        if (ch == KEY_DOWN)
        {
            if (highlight < 5)
                ++highlight;
        }
        else if (ch == KEY_UP)
        {
            if (highlight > 0)
                --highlight;
        }
        else if (ch == 10)
        {
            if (highlight == 0)
            {
                clear();
                attron(COLOR_PAIR(5));
                mvprintw(16, 25, "%s", "Your default ammo successfuly changed to Mace!");
                attroff(COLOR_PAIR(5));
                main_player.default_ammo = 1;
                refresh();
                napms(1500);
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
                return;
            }
            else if (highlight == 1)
            {
                if (main_player.number_of_arrow > 0)
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 25, "%s", "Your default ammo successfuly changed to Arrow!");
                    main_player.default_ammo = 2;
                    attroff(COLOR_PAIR(5));
                }
                else
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 37, "%s", "Not enough ammo!");
                    attroff(COLOR_PAIR(5));
                }

                refresh();
                napms(1500);
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
                return;
            }
            else if (highlight == 2)
            {
                if (main_player.number_of_sword > 0)
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 25, "%s", "Your default ammo successfuly changed to Sword!");
                    attroff(COLOR_PAIR(5));
                    main_player.default_ammo = 3;
                }
                else
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 37, "%s", "Not enough ammo!");
                    attroff(COLOR_PAIR(5));
                }

                refresh();
                napms(1500);
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
                return;
            }
            else if (highlight == 3)
            {
                if (main_player.number_of_wand > 0)
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 25, "%s", "Your default ammo successfuly changed to Wand!");
                    attroff(COLOR_PAIR(5));
                    main_player.default_ammo = 4;
                }
                else
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 37, "%s", "Not enough ammo!");
                    attroff(COLOR_PAIR(5));
                }
                refresh();
                napms(1500);
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
                return;
            }
            else if (highlight == 4)
            {
                if (main_player.number_of_dagger > 0)
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 25, "%s", "Your default ammo successfuly changed to Dagger!");
                    attroff(COLOR_PAIR(5));
                    main_player.default_ammo = 5;
                }
                else
                {
                    clear();
                    attron(COLOR_PAIR(5));
                    mvprintw(16, 37, "%s", "Not enough ammo!");
                    attroff(COLOR_PAIR(5));
                }
                refresh();
                napms(1500);
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
                return;
            }
            else if (highlight == 5)
            {
                return;
                clear();
                noecho();
                keypad(stdscr, TRUE);
                curs_set(0);
            }
        }
    }
}

int calculate_nearer(point p1, point p2, point p3, point p4)
{
    if (SQUARE(p1.x - p2.x) + SQUARE(p1.y - p2.y) >= SQUARE(p3.x - p4.x) + SQUARE(p3.y - p4.y))
        return 1;
    return 0;
}

void move_enemy()
{
    int dx[4] = {1, 0, -1, 0};
    int dy[4] = {0, 1, 0, -1};

    for (int i = 0; i < main_map.num_enemies; ++i)
    {

        if ((enemies[i].move_counter == 0) && (enemies[i].type == 3 || enemies[i].type == 5) &&
            ((SQUARE(main_player.position.x - enemies[i].position.x) + SQUARE(main_player.position.y - enemies[i].position.y)) <= 2))
        {
            enemies[i].move_counter = 5;
        }

        if (enemies[i].num_room == recognize_rooms(main_player.position))
        {

            if (enemies[i].type == 1)
            {
                if (enemies[i].health > 0 && enemies[i].available_move)
                {
                    if (enemies[i].move_counter > 0)
                    {
                        --enemies[i].move_counter;
                        for (int j = 0; j < 4; ++j)
                        {
                            point p;
                            p.x = enemies[i].position.x + dx[j];
                            p.y = enemies[i].position.y + dy[j];
                            if (calculate_nearer(enemies[i].position, main_player.position, p, main_player.position))
                            {
                                if (main_map.signed_map[p.x][p.y] != '-' && main_map.signed_map[p.x][p.y] != '|' &&
                                    main_map.signed_map[p.x][p.y] != 'o' && main_map.signed_map[p.x][p.y] != 'R' &&
                                    main_map.signed_map[p.x][p.y] != 'G' && main_map.signed_map[p.x][p.y] != '+' &&
                                    main_map.signed_map[p.x][p.y] != 'h' && (p.x != main_player.position.x || p.y != main_player.position.y))
                                {

                                    int same_location = 0;
                                    for (int k = 0; k < main_map.num_enemies; ++k)
                                    {
                                        if (p.x == enemies[k].position.x && p.y == enemies[k].position.y && k != i)
                                        {
                                            same_location = 1;
                                            break;
                                        }
                                    }

                                    if (same_location == 0)
                                    {
                                        enemies[i].position.x = p.x;
                                        enemies[i].position.y = p.y;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        point p;
                        p.x = enemies[i].position.x + dx[1];
                        p.y = enemies[i].position.y + dy[1];
                        if (main_map.signed_map[p.x][p.y] != '-' && main_map.signed_map[p.x][p.y] != '|' &&
                            main_map.signed_map[p.x][p.y] != 'o' && main_map.signed_map[p.x][p.y] != 'R' &&
                            main_map.signed_map[p.x][p.y] != 'G' && main_map.signed_map[p.x][p.y] != '+' &&
                            main_map.signed_map[p.x][p.y] != 'h' && (p.x != main_player.position.x || p.y != main_player.position.y))
                        {

                            int same_location = 0;
                            for (int k = 0; k < main_map.num_enemies; ++k)
                            {
                                if (p.x == enemies[k].position.x && p.y == enemies[k].position.y && k != i)
                                {
                                    same_location = 1;
                                    break;
                                }
                            }

                            if (same_location == 0)
                            {
                                enemies[i].position.x = p.x;
                                enemies[i].position.y = p.y;
                                break;
                            }
                        }
                    }
                }
            }
            else if (enemies[i].type == 2)
            {
                if (enemies[i].health > 0 && enemies[i].available_move)
                {
                    if (enemies[i].move_counter > 0)
                    {
                        --enemies[i].move_counter;
                        for (int j = 0; j < 4; ++j)
                        {
                            point p;
                            p.x = enemies[i].position.x + dx[j];
                            p.y = enemies[i].position.y + dy[j];
                            if (calculate_nearer(enemies[i].position, main_player.position, p, main_player.position))
                            {
                                if (main_map.signed_map[p.x][p.y] != '-' && main_map.signed_map[p.x][p.y] != '|' &&
                                    main_map.signed_map[p.x][p.y] != 'o' && main_map.signed_map[p.x][p.y] != 'R' &&
                                    main_map.signed_map[p.x][p.y] != 'G' && main_map.signed_map[p.x][p.y] != '+' &&
                                    main_map.signed_map[p.x][p.y] != 'h' && (p.x != main_player.position.x || p.y != main_player.position.y))
                                {

                                    int same_location = 0;
                                    for (int k = 0; k < main_map.num_enemies; ++k)
                                    {
                                        if (p.x == enemies[k].position.x && p.y == enemies[k].position.y && k != i)
                                        {
                                            same_location = 1;
                                            break;
                                        }
                                    }

                                    if (same_location == 0)
                                    {
                                        enemies[i].position.x = p.x;
                                        enemies[i].position.y = p.y;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        point p;
                        p.x = enemies[i].position.x + dx[2];
                        p.y = enemies[i].position.y + dy[2];
                        if (main_map.signed_map[p.x][p.y] != '-' && main_map.signed_map[p.x][p.y] != '|' &&
                            main_map.signed_map[p.x][p.y] != 'o' && main_map.signed_map[p.x][p.y] != 'R' &&
                            main_map.signed_map[p.x][p.y] != 'G' && main_map.signed_map[p.x][p.y] != '+' &&
                            main_map.signed_map[p.x][p.y] != 'h' && (p.x != main_player.position.x || p.y != main_player.position.y))
                        {

                            int same_location = 0;
                            for (int k = 0; k < main_map.num_enemies; ++k)
                            {
                                if (p.x == enemies[k].position.x && p.y == enemies[k].position.y && k != i)
                                {
                                    same_location = 1;
                                    break;
                                }
                            }

                            if (same_location == 0)
                            {
                                enemies[i].position.x = p.x;
                                enemies[i].position.y = p.y;
                                break;
                            }
                        }
                    }
                }
            }
            else if (enemies[i].type == 3)
            {
                if (enemies[i].health > 0 && enemies[i].available_move)
                {
                    if (enemies[i].move_counter > 0)
                    {
                        --enemies[i].move_counter;
                        for (int j = 0; j < 4; ++j)
                        {
                            point p;
                            p.x = enemies[i].position.x + dx[j];
                            p.y = enemies[i].position.y + dy[j];
                            if (calculate_nearer(enemies[i].position, main_player.position, p, main_player.position))
                            {
                                if (main_map.signed_map[p.x][p.y] != '-' && main_map.signed_map[p.x][p.y] != '|' &&
                                    main_map.signed_map[p.x][p.y] != 'o' && main_map.signed_map[p.x][p.y] != 'R' &&
                                    main_map.signed_map[p.x][p.y] != 'G' && main_map.signed_map[p.x][p.y] != '+' &&
                                    main_map.signed_map[p.x][p.y] != 'h' && (p.x != main_player.position.x || p.y != main_player.position.y))
                                {

                                    int same_location = 0;
                                    for (int k = 0; k < main_map.num_enemies; ++k)
                                    {
                                        if (p.x == enemies[k].position.x && p.y == enemies[k].position.y && k != i)
                                        {
                                            same_location = 1;
                                            break;
                                        }
                                    }

                                    if (same_location == 0)
                                    {
                                        enemies[i].position.x = p.x;
                                        enemies[i].position.y = p.y;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (enemies[i].type == 4)
            {
                if (enemies[i].health > 0 && enemies[i].available_move)
                {
                    if (enemies[i].move_counter > 0)
                    {
                        --enemies[i].move_counter;
                        for (int j = 0; j < 4; ++j)
                        {
                            point p;
                            p.x = enemies[i].position.x + dx[j];
                            p.y = enemies[i].position.y + dy[j];
                            if (calculate_nearer(enemies[i].position, main_player.position, p, main_player.position))
                            {
                                if (main_map.signed_map[p.x][p.y] != '-' && main_map.signed_map[p.x][p.y] != '|' &&
                                    main_map.signed_map[p.x][p.y] != 'o' && main_map.signed_map[p.x][p.y] != 'R' &&
                                    main_map.signed_map[p.x][p.y] != 'G' && main_map.signed_map[p.x][p.y] != '+' &&
                                    main_map.signed_map[p.x][p.y] != 'h' && (p.x != main_player.position.x || p.y != main_player.position.y))
                                {

                                    int same_location = 0;
                                    for (int k = 0; k < main_map.num_enemies; ++k)
                                    {
                                        if (p.x == enemies[k].position.x && p.y == enemies[k].position.y && k != i)
                                        {
                                            same_location = 1;
                                            break;
                                        }
                                    }

                                    if (same_location == 0)
                                    {
                                        enemies[i].position.x = p.x;
                                        enemies[i].position.y = p.y;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (enemies[i].type == 5)
            {
                if (enemies[i].health > 0 && enemies[i].available_move)
                {
                    if (enemies[i].move_counter > 0)
                    {
                        --enemies[i].move_counter;

                        for (int j = 0; j < 4; ++j)
                        {
                            point p;
                            p.x = enemies[i].position.x + dx[j];
                            p.y = enemies[i].position.y + dy[j];
                            if (calculate_nearer(enemies[i].position, main_player.position, p, main_player.position))
                            {
                                if (main_map.signed_map[p.x][p.y] != '-' && main_map.signed_map[p.x][p.y] != '|' &&
                                    main_map.signed_map[p.x][p.y] != 'o' && main_map.signed_map[p.x][p.y] != 'R' &&
                                    main_map.signed_map[p.x][p.y] != 'G' && main_map.signed_map[p.x][p.y] != '+' &&
                                    main_map.signed_map[p.x][p.y] != 'h' && (p.x != main_player.position.x || p.y != main_player.position.y))
                                {

                                    int same_location = 0;
                                    for (int k = 0; k < main_map.num_enemies; ++k)
                                    {
                                        if (p.x == enemies[k].position.x && p.y == enemies[k].position.y && k != i)
                                        {
                                            same_location = 1;
                                            break;
                                        }
                                    }

                                    if (same_location == 0)
                                    {
                                        enemies[i].position.x = p.x;
                                        enemies[i].position.y = p.y;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void damage_to_enemy()
{
    /*
    1 --> mace
    2 --> arrow
    3 --> sword
    4 --> wnad
    5 --> dagger
    0 --> nothing
    */

    if (main_player.default_ammo == 0)
    {
        clear();
        attron(COLOR_PAIR(6));
        mvprintw(1, 1, "%s", "No default ammo");
        attroff(COLOR_PAIR(6));
        refresh();
        return;
    }
    else if (main_player.default_ammo == 1)
    {
        for (int i = 0; i < main_map.num_enemies; ++i)
        {
            if (enemies[i].health > 0)
            {
                if ((SQUARE(main_player.position.x - enemies[i].position.x) + SQUARE(main_player.position.y - enemies[i].position.y)) <= 2)
                {

                    if (main_player.is_damage_enchant_active == 1)
                    {
                        enemies[i].health -= 10;
                    }
                    else
                    {
                        enemies[i].health -= 5;
                    }

                    main_player.last_shot = 1;

                    if (enemies[i].type == 1)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Deamon(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Deamon died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 2)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Fire Breathing Monster(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Fire Breathing Monster died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 3)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Giant(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Giant died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 4)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Snake died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 5)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Undeed(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Undeed died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                }
            }
        }
    }
    else if (main_player.default_ammo == 3)
    {
        for (int i = 0; i < main_map.num_enemies; ++i)
        {
            if (enemies[i].health > 0)
            {
                if ((SQUARE(main_player.position.x - enemies[i].position.x) + SQUARE(main_player.position.y - enemies[i].position.y)) <= 2)
                {

                    if (main_player.is_damage_enchant_active == 1)
                    {
                        enemies[i].health -= 20;
                    }
                    else
                    {
                        enemies[i].health -= 10;
                    }

                    main_player.last_shot = 3;

                    if (enemies[i].type == 1)
                    {
                        if (enemies[i].health > 0)
                        {
                            attron(COLOR_PAIR(6));
                            clear();
                            mvprintw(1, 1, "You hit the Deamon(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Deamon died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 2)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Fire Breathing Monster(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Fire Breathing Monster died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 3)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Giant(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Giant died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 4)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            attron(COLOR_PAIR(6));
                            clear();
                            mvprintw(1, 1, "%s", "Snake died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 5)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Undeed(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Undeed died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                }
            }
        }
    }
    else
    {

        int breaked = 0;
        int sth = 10;
        nodelay(stdscr, TRUE);
        time_t current, start;
        start = time(NULL);

        while (sth != 'y' && sth != 'Y' && sth != 'u' && sth != 'U' && sth != 'h' && sth != 'H' && sth != 'j' && sth != 'J' && sth != 'k' && sth != 'K' && sth != 'l' && sth != 'L' && sth != 'b' && sth != 'B' && sth != 'n' && sth != 'N')
        {

            current = time(NULL);

            if (difftime(current, start) > 3.0)
            {
                breaked = 1;
                break;
            }

            sth = getch();
            if (sth != ERR)
            {
                break;
            }

            current = time(NULL);
            if (difftime(current, start) > 3.0)
            {
                breaked = 1;
                break;
            }
        }

        nodelay(stdscr, FALSE);

        if (breaked)
            return;
        sth = (sth < 91) ? (sth + 32) : sth;

        int delta[3] = {1, 0, -1};
        int dx = 0;
        int dy = 0;
        int direction = 0;

        if (sth == 'y')
        {
            dx = delta[2];
            dy = delta[2];
            direction = 8;
        }
        else if (sth == 'u')
        {
            dx = delta[0];
            dy = delta[2];
            direction = 2;
        }
        else if (sth == 'h')
        {
            dx = delta[2];
            dy = delta[1];
            direction = 7;
        }
        else if (sth == 'j')
        {
            dx = delta[1];
            dy = delta[2];
            direction = 1;
        }
        else if (sth == 'k')
        {
            dx = delta[1];
            dy = delta[0];
            direction = 5;
        }
        else if (sth == 'l')
        {
            dx = delta[0];
            dy = delta[1];
            direction = 3;
        }
        else if (sth == 'b')
        {
            dx = delta[2];
            dy = delta[0];
            direction = 6;
        }
        else if (sth == 'n')
        {
            dx = delta[0];
            dy = delta[0];
            direction = 4;
        }

        point ammo_position;
        ammo_position.x = main_player.position.x;
        ammo_position.y = main_player.position.y;

        if (main_player.default_ammo == 2)
        {

            if (main_player.number_of_arrow > 0)
            {
                main_player.last_shot = 20 + direction;
                --main_player.number_of_arrow;

                for (int counter = 0; counter < 5; ++counter)
                {
                    int shot = 0;
                    for (int j = 0; j < main_map.num_enemies; ++j)
                    {
                        if (ammo_position.x + dx == enemies[j].position.x && ammo_position.y + dy == enemies[j].position.y)
                        {
                            shot = 1;
                            if (main_player.is_damage_enchant_active == 1)
                            {
                                enemies[j].health -= 10;
                            }
                            else
                            {
                                enemies[j].health -= 5;
                            }

                            if (enemies[j].type == 1)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Deamon(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Deamon died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 2)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Fire Breathing Monster(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Fire Breathing Monster died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 3)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Giant(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Giant died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 4)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Snake died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 5)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Undeed(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Undeed died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            break;
                        }
                    }

                    if (shot == 1)
                        break;

                    if (main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '-' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '|' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '+' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'h' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'R' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'G' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'o' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '&' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == ' ')
                    {

                        main_map.signed_map[ammo_position.x][ammo_position.y] = '6';
                        clear();
                        attron(COLOR_PAIR(5));
                        mvprintw(1, 1, "%s", "Your shot missed!");
                        attroff(COLOR_PAIR(5));
                        refresh();
                    }
                    else
                    {
                        ammo_position.x += dx;
                        ammo_position.y += dy;
                    }

                    if (counter != 4)
                    {
                        clear();
                        print_game();
                        attron(COLOR_PAIR(7));
                        mvprintw(ammo_position.y + 2, ammo_position.x + 3, "%s", "\u27B3");
                        attroff(COLOR_PAIR(7));
                        refresh();
                        napms(30);
                        move(main_player.position.y + 2, main_player.position.x + 3);
                    }

                    if (counter == 4)
                    {
                        main_map.signed_map[ammo_position.x][ammo_position.y] = '6';
                        clear();
                        attron(COLOR_PAIR(5));
                        mvprintw(1, 1, "%s", "Your shot missed!");
                        attroff(COLOR_PAIR(5));
                        refresh();
                    }
                }
            }
            else
            {
                clear();
                attron(COLOR_PAIR(5));
                mvprintw(1, 1, "%s", "Not enough ammo");
                attroff(COLOR_PAIR(5));
                refresh();
            }
        }
        else if (main_player.default_ammo == 4)
        {
            if (main_player.number_of_wand > 0)
            {
                main_player.last_shot = 40 + direction;
                --main_player.number_of_wand;

                for (int counter = 0; counter < 10; ++counter)
                {
                    int shot = 0;
                    for (int j = 0; j < main_map.num_enemies; ++j)
                    {
                        if (ammo_position.x + dx == enemies[j].position.x && ammo_position.y + dy == enemies[j].position.y)
                        {
                            shot = 1;
                            if (main_player.is_damage_enchant_active == 1)
                            {
                                enemies[j].health -= 30;
                            }
                            else
                            {
                                enemies[j].health -= 15;
                            }

                            enemies[j].available_move = 0;
                            if (enemies[j].type == 1)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Deamon(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Deamon died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 2)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Fire Breathing Monster(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Fire Breathing Monster died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 3)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Giant(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Giant died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 4)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Snake died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 5)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Snake died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            break;
                        }
                    }

                    if (shot == 1)
                        break;

                    if (main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '-' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '|' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '+' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'h' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'R' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'G' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'o' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '&' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == ' ')
                    {
                        main_map.signed_map[ammo_position.x][ammo_position.y] = '8';
                    }
                    else
                    {
                        ammo_position.x += dx;
                        ammo_position.y += dy;
                    }

                    if (counter != 9)
                    {
                        clear();
                        print_game();
                        attron(COLOR_PAIR(7));
                        mvprintw(ammo_position.y + 2, ammo_position.x + 3, "%s", "\u269A");
                        attroff(COLOR_PAIR(7));
                        refresh();
                        napms(30);
                        move(main_player.position.y + 2, main_player.position.x + 3);
                    }

                    if (counter == 9)
                    {
                        main_map.signed_map[ammo_position.x][ammo_position.y] = '8';
                        clear();
                        attron(COLOR_PAIR(5));
                        mvprintw(1, 1, "%s", "Your shot missed!");
                        attroff(COLOR_PAIR(5));
                        refresh();
                    }
                }
            }
            else
            {
                clear();
                attron(COLOR_PAIR(5));
                mvprintw(1, 1, "%s", "Not enough ammo");
                attroff(COLOR_PAIR(5));
                refresh();
            }
        }
        else if (main_player.default_ammo == 5)
        {
            if (main_player.number_of_dagger > 0)
            {
                main_player.last_shot = 50 + direction;
                --main_player.number_of_dagger;

                for (int counter = 0; counter < 5; ++counter)
                {
                    int shot = 0;
                    for (int j = 0; j < main_map.num_enemies; ++j)
                    {
                        if (ammo_position.x + dx == enemies[j].position.x && ammo_position.y + dy == enemies[j].position.y)
                        {
                            shot = 1;
                            if (main_player.is_damage_enchant_active == 1)
                            {
                                enemies[j].health -= 24;
                            }
                            else
                            {
                                enemies[j].health -= 12;
                            }

                            if (enemies[j].type == 1)
                            {
                                if (enemies[j].health > 0)
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Deamon(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Deamon died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 2)
                            {
                                if (enemies[j].health > 0)
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Fire Breathing Monster(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Fire Breathing Monster died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 3)
                            {
                                if (enemies[j].health > 0)
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Giant(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Giant died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 4)
                            {
                                if (enemies[j].health > 0)
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Snake died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 5)
                            {
                                if (enemies[j].health > 0)
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Snake died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            break;
                        }
                    }

                    if (shot == 1)
                        break;

                    if (main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '-' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '|' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '+' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'h' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'R' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'G' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'o' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '&' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == ' ')
                    {
                        main_map.signed_map[ammo_position.x][ammo_position.y] = '7';
                    }
                    else
                    {
                        ammo_position.x += dx;
                        ammo_position.y += dy;
                    }

                    if (counter != 4)
                    {
                        clear();
                        print_game();
                        attron(COLOR_PAIR(7));
                        mvprintw(ammo_position.y + 2, ammo_position.x + 3, "%s", "\u2020");
                        attroff(COLOR_PAIR(7));
                        refresh();
                        napms(30);
                        move(main_player.position.y + 2, main_player.position.x + 3);
                    }

                    if (counter == 4)
                    {
                        main_map.signed_map[ammo_position.x][ammo_position.y] = '7';
                        clear();
                        attron(COLOR_PAIR(5));
                        mvprintw(1, 1, "%s", "Your shot missed!");
                        attroff(COLOR_PAIR(5));
                        refresh();
                    }
                }
            }
            else
            {
                clear();
                attron(COLOR_PAIR(5));
                mvprintw(1, 1, "%s", "Not enough ammo");
                attroff(COLOR_PAIR(5));
                refresh();
            }
        }
    }
}

void damage_to_player()
{
    for (int i = 0; i < main_map.num_enemies; ++i)
    {
        if ((SQUARE(main_player.position.x - enemies[i].position.x) + SQUARE(main_player.position.y - enemies[i].position.y)) <= 2)
        {
            if (enemies[i].health > 0)
            {
                main_player.last_attack = time(NULL);
                if (enemies[i].type == 1)
                {
                    if (main_player.health >= 1)
                        main_player.health -= 1;
                    else
                        main_player.health = 0;
                    attron(COLOR_PAIR(5));
                    mvprintw(1, 1, "%s", "The Deamon hit you");
                    attroff(COLOR_PAIR(5));
                    refresh();
                }
                else if (enemies[i].type == 2)
                {
                    if (main_player.health >= 2)
                        main_player.health -= 2;
                    else
                        main_player.health = 0;
                    attron(COLOR_PAIR(5));
                    mvprintw(1, 1, "%s", "The Fire Breathing Monster hit you");
                    attroff(COLOR_PAIR(5));
                    refresh();
                }
                else if (enemies[i].type == 3)
                {
                    if (main_player.health >= 3)
                        main_player.health -= 3;
                    else
                        main_player.health = 0;
                    attron(COLOR_PAIR(5));
                    mvprintw(1, 1, "%s", "The Giant hit you");
                    attroff(COLOR_PAIR(5));
                    refresh();
                }
                else if (enemies[i].type == 4)
                {
                    if (main_player.health >= 4)
                        main_player.health -= 4;
                    else
                        main_player.health = 0;
                    attron(COLOR_PAIR(5));
                    mvprintw(1, 1, "%s", "The Snake hit you");
                    attroff(COLOR_PAIR(5));
                    refresh();
                }
                else if (enemies[i].type == 5)
                {
                    if (main_player.health >= 5)
                        main_player.health -= 5;
                    else
                        main_player.health = 0;
                    attron(COLOR_PAIR(5));
                    mvprintw(1, 1, "%s", "The Undeed hit you");
                    attroff(COLOR_PAIR(5));
                    refresh();
                }
            }
        }
    }
}

void last_shot()
{

    if (main_player.last_shot == 1)
    {

        for (int i = 0; i < main_map.num_enemies; ++i)
        {
            if (enemies[i].health > 0)
            {
                if ((SQUARE(main_player.position.x - enemies[i].position.x) + SQUARE(main_player.position.y - enemies[i].position.y)) <= 2)
                {

                    if (main_player.is_damage_enchant_active == 1)
                    {
                        enemies[i].health -= 10;
                    }
                    else
                    {
                        enemies[i].health -= 5;
                    }

                    if (enemies[i].type == 1)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Deamon(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Deamon died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 2)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Fire Breathing Monster(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Fire Breathing Monster died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 3)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Giant(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Giant died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 4)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Snake died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 5)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Undeed(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Undeed died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                }
            }
        }
    }
    else if (main_player.last_shot == 3)
    {

        for (int i = 0; i < main_map.num_enemies; ++i)
        {
            if (enemies[i].health > 0)
            {
                if ((SQUARE(main_player.position.x - enemies[i].position.x) + SQUARE(main_player.position.y - enemies[i].position.y)) <= 2)
                {

                    if (main_player.is_damage_enchant_active == 1)
                    {
                        enemies[i].health -= 20;
                    }
                    else
                    {
                        enemies[i].health -= 10;
                    }

                    if (enemies[i].type == 1)
                    {
                        if (enemies[i].health > 0)
                        {
                            attron(COLOR_PAIR(6));
                            clear();
                            mvprintw(1, 1, "You hit the Deamon(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Deamon died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 2)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Fire Breathing Monster(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Fire Breathing Monster died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 3)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Giant(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Giant died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 4)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            attron(COLOR_PAIR(6));
                            clear();
                            mvprintw(1, 1, "%s", "Snake died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                    else if (enemies[i].type == 5)
                    {
                        if (enemies[i].health > 0)
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "You hit the Undeed(Health : %d)", enemies[i].health);
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                        else
                        {
                            clear();
                            attron(COLOR_PAIR(6));
                            mvprintw(1, 1, "%s", "Undeed died by your shot!");
                            attroff(COLOR_PAIR(6));
                            refresh();
                        }
                    }
                }
            }
        }
    }
    else
    {

        int sth = main_player.last_shot % 10;

        int delta[3] = {1, 0, -1};
        int dx = 0;
        int dy = 0;

        if (sth == 8)
        {
            dx = delta[2];
            dy = delta[2];
        }
        else if (sth == 2)
        {
            dx = delta[0];
            dy = delta[2];
        }
        else if (sth == 7)
        {
            dx = delta[2];
            dy = delta[1];
        }
        else if (sth == 1)
        {
            dx = delta[1];
            dy = delta[2];
        }
        else if (sth == 5)
        {
            dx = delta[1];
            dy = delta[0];
        }
        else if (sth == 3)
        {
            dx = delta[0];
            dy = delta[1];
        }
        else if (sth == 6)
        {
            dx = delta[2];
            dy = delta[0];
        }
        else if (sth == 4)
        {
            dx = delta[0];
            dy = delta[0];
        }

        point ammo_position;
        ammo_position.x = main_player.position.x;
        ammo_position.y = main_player.position.y;

        if (main_player.last_shot / 10 == 2)
        {

            if (main_player.number_of_arrow > 0)
            {
                --main_player.number_of_arrow;

                for (int counter = 0; counter < 5; ++counter)
                {
                    int shot = 0;
                    for (int j = 0; j < main_map.num_enemies; ++j)
                    {
                        if (ammo_position.x + dx == enemies[j].position.x && ammo_position.y + dy == enemies[j].position.y)
                        {
                            shot = 1;
                            if (main_player.is_damage_enchant_active == 1)
                            {
                                enemies[j].health -= 10;
                            }
                            else
                            {
                                enemies[j].health -= 5;
                            }

                            if (enemies[j].type == 1)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Deamon(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Deamon died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 2)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Fire Breathing Monster(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Fire Breathing Monster died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 3)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Giant(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Giant died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 4)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Snake died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 5)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Undeed(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Undeed died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            break;
                        }
                    }

                    if (shot == 1)
                        break;

                    if (main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '-' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '|' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '+' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'h' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'R' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'G' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'o' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '&' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == ' ')
                    {

                        main_map.signed_map[ammo_position.x][ammo_position.y] = '6';
                        clear();
                        attron(COLOR_PAIR(5));
                        mvprintw(1, 1, "%s", "Your shot missed!");
                        attroff(COLOR_PAIR(5));
                        refresh();
                    }
                    else
                    {
                        ammo_position.x += dx;
                        ammo_position.y += dy;
                    }

                    if (counter != 4)
                    {
                        clear();
                        print_game();
                        attron(COLOR_PAIR(7));
                        mvprintw(ammo_position.y + 2, ammo_position.x + 3, "%s", "\u27B3");
                        attroff(COLOR_PAIR(7));
                        refresh();
                        napms(30);
                        move(main_player.position.y + 2, main_player.position.x + 3);
                    }

                    if (counter == 4)
                    {
                        main_map.signed_map[ammo_position.x][ammo_position.y] = '6';
                        clear();
                        attron(COLOR_PAIR(5));
                        mvprintw(1, 1, "%s", "Your shot missed!");
                        attroff(COLOR_PAIR(5));
                        refresh();
                    }
                }
            }
            else
            {
                clear();
                attron(COLOR_PAIR(5));
                mvprintw(1, 1, "%s", "Not enough ammo");
                attroff(COLOR_PAIR(5));
                refresh();
            }
        }
        else if (main_player.last_shot / 10 == 4)
        {
            if (main_player.number_of_wand > 0)
            {
                --main_player.number_of_wand;

                for (int counter = 0; counter < 10; ++counter)
                {
                    int shot = 0;
                    for (int j = 0; j < main_map.num_enemies; ++j)
                    {
                        if (ammo_position.x + dx == enemies[j].position.x && ammo_position.y + dy == enemies[j].position.y)
                        {
                            shot = 1;
                            if (main_player.is_damage_enchant_active == 1)
                            {
                                enemies[j].health -= 30;
                            }
                            else
                            {
                                enemies[j].health -= 15;
                            }

                            enemies[j].available_move = 0;
                            if (enemies[j].type == 1)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Deamon(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Deamon died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 2)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Fire Breathing Monster(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Fire Breathing Monster died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 3)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Giant(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Giant died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 4)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Snake died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 5)
                            {
                                if (enemies[j].health > 0)
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Snake died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            break;
                        }
                    }

                    if (shot == 1)
                        break;

                    if (main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '-' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '|' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '+' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'h' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'R' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'G' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'o' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '&' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == ' ')
                    {
                        main_map.signed_map[ammo_position.x][ammo_position.y] = '8';
                    }
                    else
                    {
                        ammo_position.x += dx;
                        ammo_position.y += dy;
                    }

                    if (counter != 9)
                    {
                        clear();
                        print_game();
                        attron(COLOR_PAIR(7));
                        mvprintw(ammo_position.y + 2, ammo_position.x + 3, "%s", "\u269A");
                        attroff(COLOR_PAIR(7));
                        refresh();
                        napms(30);
                        move(main_player.position.y + 2, main_player.position.x + 3);
                    }

                    if (counter == 9)
                    {
                        main_map.signed_map[ammo_position.x][ammo_position.y] = '8';
                        clear();
                        attron(COLOR_PAIR(5));
                        mvprintw(1, 1, "%s", "Your shot missed!");
                        attroff(COLOR_PAIR(5));
                        refresh();
                    }
                }
            }
            else
            {
                clear();
                attron(COLOR_PAIR(5));
                mvprintw(1, 1, "%s", "Not enough ammo");
                attroff(COLOR_PAIR(5));
                refresh();
            }
        }
        else if (main_player.last_shot / 10 == 5)
        {
            if (main_player.number_of_dagger > 0)
            {
                --main_player.number_of_dagger;

                for (int counter = 0; counter < 5; ++counter)
                {
                    int shot = 0;
                    for (int j = 0; j < main_map.num_enemies; ++j)
                    {
                        if (ammo_position.x + dx == enemies[j].position.x && ammo_position.y + dy == enemies[j].position.y)
                        {
                            shot = 1;
                            if (main_player.is_damage_enchant_active == 1)
                            {
                                enemies[j].health -= 24;
                            }
                            else
                            {
                                enemies[j].health -= 12;
                            }

                            if (enemies[j].type == 1)
                            {
                                if (enemies[j].health > 0)
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Deamon(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Deamon died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 2)
                            {
                                if (enemies[j].health > 0)
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Fire Breathing Monster(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Fire Breathing Monster died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 3)
                            {
                                if (enemies[j].health > 0)
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Giant(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Giant died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 4)
                            {
                                if (enemies[j].health > 0)
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Snake died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            else if (enemies[j].type == 5)
                            {
                                if (enemies[j].health > 0)
                                {
                                    clear();
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "You hit the Snake(Health : %d)", enemies[j].health);
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                                else
                                {
                                    attron(COLOR_PAIR(6));
                                    mvprintw(1, 1, "%s", "Snake died by your shot!");
                                    attroff(COLOR_PAIR(6));
                                    refresh();
                                }
                            }
                            break;
                        }
                    }

                    if (shot == 1)
                        break;

                    if (main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '-' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '|' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '+' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'h' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'R' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'G' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == 'o' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == '&' ||
                        main_map.signed_map[ammo_position.x + dx][ammo_position.y + dy] == ' ')
                    {
                        main_map.signed_map[ammo_position.x][ammo_position.y] = '7';
                    }
                    else
                    {
                        ammo_position.x += dx;
                        ammo_position.y += dy;
                    }

                    if (counter != 4)
                    {
                        clear();
                        print_game();
                        attron(COLOR_PAIR(7));
                        mvprintw(ammo_position.y + 2, ammo_position.x + 3, "%s", "\u2020");
                        attroff(COLOR_PAIR(7));
                        refresh();
                        napms(30);
                        move(main_player.position.y + 2, main_player.position.x + 3);
                    }

                    if (counter == 4)
                    {
                        main_map.signed_map[ammo_position.x][ammo_position.y] = '7';
                        clear();
                        attron(COLOR_PAIR(5));
                        mvprintw(1, 1, "%s", "Your shot missed!");
                        attroff(COLOR_PAIR(5));
                        refresh();
                    }
                }
            }
            else
            {
                clear();
                attron(COLOR_PAIR(5));
                mvprintw(1, 1, "%s", "Not enough ammo");
                attroff(COLOR_PAIR(5));
                refresh();
            }
        }
    }
}

void check_enemy_killed()
{
    for (int i = 0; i < main_map.num_enemies; ++i)
    {
        if (enemies[i].health <= 0)
        {
            enemies[i].position.x = 91;
            enemies[i].position.y = 31;
        }
    }
}

int make_new_game()
{

    if (strcmp(GENERAL_SETTING.USERNAME, "Guest") != 0)
    {

        int index = 0;
        for (int i = 0; i < main_scoreboard.num_users; ++i)
        {
            if (strcmp(GENERAL_SETTING.USERNAME, main_scoreboard.usernames[i]) == 0)
            {
                index = i;
                break;
            }
        }

        main_scoreboard.num_played_games[index] += 1;

        if (main_scoreboard.experience[index] == 0)
        {
            main_scoreboard.first[index] = time(NULL);
            main_scoreboard.experience[index] = 1;
        }
        else
        {
            time_t delta;
            delta = time(NULL) - main_scoreboard.first[index];
            int delta_experience = (int)delta / 86400;
            main_scoreboard.experience[index] += delta_experience;
            main_scoreboard.first[index] = time(NULL);
        }

        FILE *scoreboard_file = fopen("scoreboard.bin", "wb");
        fwrite(&main_scoreboard, sizeof(users_scoreboard), 1, scoreboard_file);
        fclose(scoreboard_file);
    }

    create_map();
    initialize_enemies();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    int ch;
    int pick_up = 1;
    clear();
    setup_player(&main_player);
    initialize_random_position(&main_player);
    print_game();

    while (1)
    {

        ch = getch();

        switch (ch)
        {
        case 'Y':
        case 'y':
        case 'U':
        case 'u':
        case 'H':
        case 'h':
        case 'J':
        case 'j':
        case 'K':
        case 'k':
        case 'L':
        case 'l':
        case 'B':
        case 'b':
        case 'N':
        case 'n':
        case 'F':
        case 'f':
        {
            ch = (ch < 91) ? (ch + 32) : ch;

            if (ch != 'f')
            {
                pick_up = player_movement(ch, pick_up);
                if (main_player.is_speed_enchant_active == 1)
                    pick_up = player_movement(ch, pick_up);
                move_enemy();
                damage_to_player();
                print_game();
                if (end_game() == -1)
                    return 0;
            }
            else
            {
                if (main_map.floor != 5 && GENERAL_SETTING.LEVEL != 3)
                {
                    ch = (ch < 91) ? (ch + 32) : ch;
                    pick_up = player_movement(ch, pick_up);
                    if (main_player.is_speed_enchant_active == 1)
                        pick_up = player_movement(ch, pick_up);
                    move_enemy();
                    damage_to_player();
                    print_game();
                    if (end_game() == -1)
                        return 0;
                }
            }
        }
        break;

        case 'G':
        case 'g':
            pick_up = 0;
            break;

        case 'S':
        case 's':
        {
            search_around();
            move_enemy();
            damage_to_player();
            print_game();
            if (end_game() == -1)
                return 0;
        }
        break;

        case 'X':
        case 'x':
        {
            list_of_enchant();
            clear();
            print_game();
        }
        break;

        case 'I':
        case 'i':
        {
            list_of_ammos();
            clear();
            print_game();
        }
        break;

        case 'M':
        case 'm':
        {
            if (main_map.floor != 5 && GENERAL_SETTING.LEVEL != 3)
            {
                view_whole_map();
                flushinp();
                move_enemy();
                damage_to_player();
                print_game();
                if (end_game() == -1)
                    return 0;
            }
        }

        break;

        case 'E':
        case 'e':
        {
            consume_food();
            print_game();
        }
        break;

        case 'A':
        case 'a':
        {
            if (main_player.last_shot != 0)
            {
                move_enemy();
                damage_to_player();
                last_shot();
                check_enemy_killed();
                print_game();
                if (end_game() == -1)
                    return 0;
            }
        }
        break;
        case KEY_RIGHT:
            next_floor();
            break;

        case 10:
        {
            time_t time1 = time(NULL);
            int result = pause_menu();
            time_t time2 = time(NULL);
            if (result == 0)
            {
                return 0;
            }
            else if (result == 1)
            {
                save_game();
                clear();
                print_game();
                if (end_game() == -1)
                    return 0;
            }
            else
            {
                clear();
                print_game();
            }

            main_player.last_time += time2 - time1;
            main_player.last_attack += time2 - time1;
            main_player.last_password += time2 - time1;
        }
        break;

        case 32:
        {
            damage_to_enemy();
            damage_to_player();
            move_enemy();
            check_enemy_killed();
            print_game();
        }
        break;

        default:
            break;
        }
    }
}

int resume_last_game()
{
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    int ch;
    int pick_up = 1;
    clear();
    print_game();

    while (1)
    {

        ch = getch();

        switch (ch)
        {
        case 'Y':
        case 'y':
        case 'U':
        case 'u':
        case 'H':
        case 'h':
        case 'J':
        case 'j':
        case 'K':
        case 'k':
        case 'L':
        case 'l':
        case 'B':
        case 'b':
        case 'N':
        case 'n':
        case 'F':
        case 'f':
        {
            ch = (ch < 91) ? (ch + 32) : ch;

            if (ch != 'f')
            {
                pick_up = player_movement(ch, pick_up);
                if (main_player.is_speed_enchant_active == 1)
                    pick_up = player_movement(ch, pick_up);
                move_enemy();
                damage_to_player();
                print_game();
                if (end_game() == -1)
                    return 0;
            }
            else
            {
                if (main_map.floor != 5 && GENERAL_SETTING.LEVEL != 3)
                {
                    ch = (ch < 91) ? (ch + 32) : ch;
                    pick_up = player_movement(ch, pick_up);
                    if (main_player.is_speed_enchant_active == 1)
                        pick_up = player_movement(ch, pick_up);
                    move_enemy();
                    damage_to_player();
                    print_game();
                    if (end_game() == -1)
                        return 0;
                }
            }
        }
        break;

        case 'G':
        case 'g':
            pick_up = 0;
            break;

        case 'S':
        case 's':
        {
            search_around();
            move_enemy();
            damage_to_player();
            print_game();
            if (end_game() == -1)
                return 0;
        }
        break;

        case 'X':
        case 'x':
        {
            list_of_enchant();
            clear();
            print_game();
        }
        break;

        case 'I':
        case 'i':
        {
            list_of_ammos();
            clear();
            print_game();
        }
        break;

        case 'M':
        case 'm':
        {
            if (main_map.floor != 5 && GENERAL_SETTING.LEVEL != 3)
            {
                view_whole_map();
                flushinp();
                move_enemy();
                damage_to_player();
                print_game();
                if (end_game() == -1)
                    return 0;
            }
        }

        break;

        case 'E':
        case 'e':
        {
            consume_food();
            print_game();
        }
        break;

        case 'A':
        case 'a':
        {
            if (main_player.last_shot != 0)
            {
                move_enemy();
                damage_to_player();
                last_shot();
                check_enemy_killed();
                print_game();
                if (end_game() == -1)
                    return 0;
            }
        }
        break;
        case KEY_RIGHT:
            next_floor();
            break;

        case 10:
        {
            time_t time1 = time(NULL);
            int result = pause_menu();
            time_t time2 = time(NULL);
            if (result == 0)
            {
                endwin();
                exit(0);
            }
            else if (result == 1)
            {
                save_game();
                clear();
                print_game();
                if (end_game() == -1)
                    return 0;
            }
            else
            {
                clear();
                print_game();
            }

            main_player.last_time += time2 - time1;
            main_player.last_attack += time2 - time1;
            main_player.last_password += time2 - time1;
        }
        break;

        case 32:
        {
            damage_to_enemy();
            damage_to_player();
            move_enemy();
            check_enemy_killed();
            print_game();
        }
        break;

        default:
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
int main()
{
    FILE *scoreboard_file = fopen("scoreboard.bin", "rb");
    if (scoreboard_file)
    {
        fread(&main_scoreboard, sizeof(users_scoreboard), 1, scoreboard_file);
        fclose(scoreboard_file);
    }
    else
    {

        main_scoreboard.num_users = 0;
        for (int i = 0; i < MAX_NUM_USERS; ++i)
        {
            main_scoreboard.first[i] = time(NULL);
            main_scoreboard.golds[i] = 0;
            main_scoreboard.experience[i] = 0;
            main_scoreboard.num_played_games[i] = 0;
            main_scoreboard.score[i] = 0;
        }

        FILE *new_scoreboard_file = fopen("scoreboard.bin", "wb");
        fwrite(&main_scoreboard, sizeof(users_scoreboard), 1, new_scoreboard_file);
        fclose(new_scoreboard_file);
    }

    setlocale(LC_ALL, "");
    initscr();
    start_color();

    init_color(11, 750, 635, 0);      // gold
    init_color(12, 550, 550, 550);    // silver
    init_color(13, 600, 300, 150);    // bronze
    init_color(14, 300, 300, 300);    // black
    init_color(15, 1000, 0, 0);       // red
    init_color(16, 0, 1000, 0);       // green
    init_color(17, 0, 500, 1000);     // blue
    init_color(18, 1000, 1000, 0);    // yellow
    init_color(19, 1000, 400, 0);     // orange
    init_color(20, 600, 60, 900);     // purple
    init_color(21, 1000, 1000, 1000); // whole white

    init_pair(1, 11, COLOR_BLACK);
    init_pair(2, 12, COLOR_BLACK);
    init_pair(3, 13, COLOR_BLACK);
    init_pair(4, 14, COLOR_BLACK);
    init_pair(5, 15, COLOR_BLACK);
    init_pair(6, 16, COLOR_BLACK);
    init_pair(7, 17, COLOR_BLACK);
    init_pair(8, 18, COLOR_BLACK);
    init_pair(9, 19, COLOR_BLACK);
    init_pair(10, 20, COLOR_BLACK);
    init_pair(11, 21, COLOR_WHITE);

    enemies = NULL;

    initialize_general_setting();

    main_menu();

    endwin();
    return 0;
}

/*
M --> master key --> \u25B2
h --> hidden door --> \u2022
R --> locked pass door --> red @
G --> unlocked pass door --> green @
+ --> door
- --> wall
| --> wall
t --> traps --> \u2022
^ --> recognized traps
o --> pillar
& --> password
# --> corridor
g --> gold --> \u26c0
b --> black gold --> \u26c2
H --> Health enchant --> \u2695
S --> Speed enchant --> \u26f7
D --> Damage enchant --> \u2620
d --> dagger --> \u2020
w --> wand -->  \u269A
a --> arrow --> \u27B3
s --> sword --> \u2694
f --> food --> \u2299
floor --> \u2022
< --> stair
1 --> Deamon --> \U0001d507
2 --> Fire breathing monster --> \U0001d509
3 --> Giant --> \U0001D5A6
4 --> Snake --> \U0001d516
5 --> undeed --> \U0001d518
6 --> secondery arrow
7 --> secondery dagger
8 --> secondery wand
T --> treasure_room --> \U0001f5dd
*/

/*
y --> left and up
u --> right and up
h --> left
j --> up
k --> down
l--> right
b -->left and down
n --> right and down

f --> move to a obstacle
g --> don't pick up
s --> search around
right arrow --> next floor
i --> list  of ammos
enter --> setting(exit, save and exit)
M --> view the whole map
E --> foods
p --> password
x --> enchants list
space --> shot
a --> last_shot
*/