#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "map.h"

extern struct map* init_map(int rows,int cols, int fill_value);
extern void print_map();
extern void place_ships();
extern int bomb_player (int x,int y);
extern int input_and_bomb();
extern void start_screen();
extern void end_screen();
extern void win_screen();

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
 

//function to start game loop
void boot_up(){
	game_map = init_map(9, 9, ' '); 
    place_ships();
    print_map();
	//int to check if user lost
	int game_over = 1;
    while (1) {
        game_over = input_and_bomb();
        print_map();  // Print the map after each bomb attempt
		if (!game_over) {
			lose_screen();
		}
    }
}

void start_screen() {

    printf("Welcome to Battleship\nYour fleet is randomly placed throughout the combat zone\n");
    printf("Please Enter Your Initials Sailor: \n");
    
    scanf("%3s", user.name);  // Read users initials
    user.score = 0;  // Initialize score to 0

    printf("%s sailor, you are ready for combat\n", user.name);
    printf("The grid in front of you is the field of play. '@' represents one part of your battleship.\n");
    printf("You will input X and Y coordinates targeting your opponent's ship.\n");
    printf("X marks a miss, and '*' represents a hit to one of your enemy's battleships.\n");
    printf("The goal is to win in as few moves as possible. Press enter to continue, and good luck sailor!\n");

    getchar();  // Wait for the Enter key
	getchar();
    boot_up();
}


void win_screen() {
    printf("\nCongratulations, %s!\n", user.name);
    printf("You have successfully sunk all your opponent's ships!\n");
    printf("You achieved this victory in just %d moves!\n", user.score);
    printf("Your strategic skills have truly shined on the battlefield!\n");
    printf("Thank you for playing Battleship! Ready for another round?\n");
    printf("Press Enter to exit the game...\n");
}

void lose_screen() {
	printf("\nMission Failed, %s...\n", user.name);
    printf("Your fleet has been sunk by the enemy!\n");
    printf("Despite your best efforts, victory was not yours today.\n");
    printf("It took you %d moves, but the enemy's strategy prevailed.\n", user.score);
    printf("Don't lose hope, Sailor! Every defeat is a lesson learned.\n");
    printf("Prepare yourself for the next battle and come back stronger!\n");
    printf("Press Enter to return to the main menu...\n");
}



//TODO randomize 
void place_ships() {
	//srand(time(NULL));
	//int ship1x = rand() % (7 + 1 - 1) + 1;
	//int ship1y = rand() % (7 + 1 - 1) + 1;

	//ship1
	game_map -> data [1][1] = '@';
	game_map -> data [2][1] = '@';

	//ship2
	game_map -> data [3][2] = '@';
	game_map -> data [3][3] = '@';
	game_map -> data [3][4] = '@';

	//ship3
	game_map -> data [3][6] = '@';
	game_map -> data [4][6] = '@';
	game_map -> data [5][6] = '@';
	game_map -> data [6][6] = '@';

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


