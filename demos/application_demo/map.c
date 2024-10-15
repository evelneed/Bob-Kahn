#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "battleship.h"

//extern struct map* init_map(int rows,int cols, int fill_value);
extern void print_map();
extern void place_ships();
extern int bomb_player (int x,int y);
extern int* input_and_bomb();
extern void start_screen();
extern void end_screen();
extern void win_screen();
extern void boot_up();

//TODO make this random
struct ships ship1 = {
	.shipx = {1,2},
	.shipy = {1,1},
	.ship_life = 2,
};

struct ships ship2 = {
	.shipx = {3,3,3},
	.shipy = {2,3,4},
	.ship_life = 3,
};

struct ships ship3 = {
	.shipx = {3,4,5,6},
	.shipy = {6,6,6,6},
	.ship_life = 4,
};

struct score user;  // Declare a user variable of type score
 





//TODO randomize 
void place_ships() {
	//srand(time(NULL));
	//int ship1x = rand() % (7 + 1 - 1) + 1;
	//int ship1y = rand() % (7 + 1 - 1) + 1;

	//ship1
	game_map -> data [1][1] = '#';
	game_map -> data [2][1] = '#';

	//ship2
	game_map -> data [3][2] = '#';
	game_map -> data [3][3] = '#';
	game_map -> data [3][4] = '#';

	//ship3
	game_map -> data [3][6] = '#';
	game_map -> data [4][6] = '#';
	game_map -> data [5][6] = '#';
	game_map -> data [6][6] = '#';

}


void write_hits (int x,int y, int collision) {
	//change icon depending on if it hit or not
    //write to map your last hit
	if (collision) {
		game_map -> data[x][y] = '@';
		printf("DIRECT HIT \n HIT THEM AGAIN NEXT TURN");

	}
	else {
		game_map -> data[x][y] = 'X';
		printf("YOU MISSED \n GOOD LUCK NEXT TURN SAILOR \n");
	}
}

    



struct map* init_map(int rows,int cols, int fill_value) {

	struct map* map = malloc(sizeof(struct map ));
	map -> x=rows;
	map -> y=cols;
	map -> data = malloc(rows * sizeof(char*));

	int c_counter = 32; // Start from 32 (space in ASCII)


	for (int i = 0; i < rows; i++)
		map -> data[i] = malloc(sizeof(char) * cols);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			map -> data[i][j] = fill_value;
	for (int i = 0; i < rows; i++){
		int d = '0' + i;
		map -> data[i][0] = d;
		map -> data[i][rows-1] = d;
		for (int j = 0; j < cols; j++) {
			char c = j + '0';
			map -> data[0][j] = c;
			map -> data[cols-1][j] = c; 
			c_counter++;
		}
	}
	return map;
}

//make map size adjustable
void print_map() {
    int rows = game_map->x;
    int cols = game_map->y;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", game_map->data[i][j]);  // Print each character followed by a space
        }
        printf("\n");  // Print newline after each row
    }
}


