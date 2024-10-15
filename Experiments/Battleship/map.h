
#include <stdio.h>
#include <stdlib.h>

struct map {
	int x;
	int y;
	char** data;
};

struct map* game_map;  // Global variable accessible from all functions

struct ships {
	int shipx[4];
	int shipy[4];
    int ship_life;
};

struct score {
    char name[3];
    int score;
};

//add high score screen

extern struct score user;
extern struct ships ship1;
extern struct ships ship2;
extern struct ships ship3;

struct map* init_map(int rows,int cols, int fill_value);
void print_map();
void place_ships();
int bomb_player (int x,int y);
int input_and_bomb();
void start_screen();
void lose_screen();
void win_screen();
void boot_up();