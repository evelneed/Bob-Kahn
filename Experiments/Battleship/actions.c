#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"



int bomb_player (int x,int y) {
    int hit = 0;
    //check if the layout bombed hit a ship
    for (int i = 0; i < 4; i++){
        if ((x == ship1.shipx[i]) && y == ship1.shipy[i]) {
            game_map -> data[x][y] = '*';
            ship1.ship_life --;
            hit = 1;
            printf("YOU HIT A SHIP \n");
            //TODO make sure that you can only bomb each location 
            if (ship1.ship_life == 0){
                printf("You Destroyed One Of Your Opponents Ships \n");
            }
        }
        if ((x == ship2.shipx[i]) && y == ship2.shipy[i]) {
            game_map -> data[x][y] = '*';
            ship2.ship_life --;
            hit = 1;
            printf("YOU HIT A SHIP \n");
            //TODO make sure that you can only bomb each location 
            if (ship2.ship_life == 0){
                printf("You Destroyed One Of Your Opponents Ships \n");
            }
        }
        if ((x == ship3.shipx[i]) && y == ship3.shipy[i]) {
            game_map -> data[x][y] = '*';
            printf("\n Your Ship Is Hit\n");
            ship3.ship_life --;
            hit = 1;
            printf("YOU HIT A SHIP \n");
            //TODO make sure that you can only bomb each location 
            if (ship3.ship_life == 0){
                printf("You Destroyed One Of Your Opponents Ships \n");
            }
        }
    }
    if (!hit){
        //set the map as a miss
        game_map -> data[x][y] = 'X';
        //add the function that sends a message back to the player
        printf("YOU MISSED \n GOOD LUCK NEXT TURN SAILOR \n");
    }
    //add a turn onto the players score
    user.score++;
    // check if user lost 
    if ((ship1.ship_life == 0) && (ship2.ship_life == 0) && (ship3.ship_life == 0)) {
        return 0;
    }
    else {
        return 1;
    }
    
}



// Function to prompt user input and validate it
int input_and_bomb() {
    int x, y;
    while (1) {
        printf("Enter coordinates to bomb (x y): ");
        if (scanf("%d %d", &x, &y) != 2) {  // Ensure valid input
            printf("Invalid input. Please enter two integers.\n");
            while (getchar() != '\n');  // Clear input buffer
            continue;
        }
        //make sure input is within the map moundaries
        if (x >= 1 && x < game_map->x-1 && y >= 1 && y < game_map->y-1) {
            break;  // Valid input, exit the loop
        } else {
            printf("Coordinates out of bounds. Try again.\n");
        }
    }
    //flip x and y so it matches correct coordinates
    return (bomb_player(y, x));  // Call bomb_player with valid coordinates

}
