#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "link_layer.h"  // Include the header for the link layer
#include <pthread.h>
#include "battleship.h"

extern int* getMove();
extern void sendMove(int* pointers);
extern struct map* init_map(int y, int x, int c);

int game_over = 0;       // 1 = Game in progress, 0 = Game over
int player_turn = 1;     // 1 = Your turn, 0 = Opponent's turn
//your last moves turn
int* cords;
int is_hit = 0;
struct map* game_map; 

int have_lost();

void start_screen() {
	cords = (int* )malloc(sizeof(int) * 2);
    
    printf("Welcome to Battleship\nYour fleet is randomly placed throughout the combat zone\n");
    printf("Please Enter Your Initials Sailor: \n");
    
    scanf("%3s", user.name);  // Read users initials
    user.score = 0;  // Initialize score to 0

    printf("%s sailor, you are ready for combat\n", user.name);
    printf("The grid in front of you is the field of play. '#' represents one part of your battleship.\n");
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

//function to start game loop
void boot_up(){
	game_map = init_map(9, 9, ' '); 
    place_ships();
    print_map();
    while (1) {
        if (game_over) {
            //send to opponent losing message
            int output[4];
            //send blank moves so opponent knows you lost
            output[0] = 0;
            output[1] = 0;
            //set collision or not
            output[2] = 0;
            output[3] = 1;
            //send to opponent
            sendMove(output);
            lose_screen();
            break;
        }
        //YOUR move
        if (player_turn) {
            //get correct input
            cords = input_and_bomb();
            //what is sent to opponent
            int output[4];
            //set x and y
            output[0] = cords[0];
            output[1] = cords[1];
            //set collision or not
            output[2] = is_hit;
            output[3] = game_over;
            //send to opponent
            sendMove(output);
            sleep(1);
            player_turn = 0;
        }
        //receiving opponents move
        if (!player_turn) {
            //scan for input
            int* opps_moves = getMove();
            //register your opponents moves
            int x = opps_moves[0];
            int y = opps_moves[1];
            //check if your last move hit
            int collision = opps_moves[2];
            write_hits(cords[0],cords[1],collision);
            print_map();
            sleep(1);
            //check if you won the game
            if (opps_moves[3] == 1){
                  win_screen();
                  break;
            }
            printf("\n opponents move incoming...\n");
            //sleep for dramatic effect
            sleep(2);
            //check oppononts hits
            is_hit = check_opps_hits(x,y);
            game_over = have_lost();
            print_map();
        }
    }
}

int have_lost(){
    // check if you have lost 
    if ((ship1.ship_life == 0) && (ship2.ship_life == 0) && (ship3.ship_life == 0)) {
        return 1;
    }
    else {
        return 0;
    }
}


