#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "battleship.h"





int check_opps_hits (int x,int y) {
    int hit = 0;
    //check if the layout bombed hit a ship
    for (int i = 0; i < 4; i++){
        if ((x == ship1.shipx[i]) && y == ship1.shipy[i]) {
            game_map -> data[x][y] = '*';
            ship1.ship_life --;
            hit = 1;
            printf("THEY HIT A SHIP \n");
            //TODO make sure that you can only bomb each location 
            if (ship1.ship_life == 0){
                printf("One Of Your Ships Is Destroyed \n");
            }
        }
        if ((x == ship2.shipx[i]) && y == ship2.shipy[i]) {
            game_map -> data[x][y] = '*';
            ship2.ship_life --;
            hit = 1;
            printf("THEY HIT A SHIP \n");
            //TODO make sure that you can only bomb each location 
            if (ship2.ship_life == 0){
                printf("One Of Your Ships Is Destroyed \n");
            }
        }
        if ((x == ship3.shipx[i]) && y == ship3.shipy[i]) {
            game_map -> data[x][y] = '*';
            printf("\n Your Ship Is Hit\n");
            ship3.ship_life --;
            hit = 1;
            printf("THEY HIT A SHIP \n");
            //TODO make sure that you can only bomb each location 
            if (ship3.ship_life == 0){
                printf("One Of Your Ships Is Destroyed \n");
            }
        }
    }
    //add a turn onto the players score
    user.score++;
    if (!hit){
        //set the map as a miss
        game_map -> data[x][y] = 'X';
        //add the function that sends a message back to the player
        printf("THE ENEMY MISSED \n YOU GOT LUCKY THIS TIME \n");
        return 0;
    }
    else {
        return 1;
    }

    
}


// Function to prompt user input and validate it
int* input_and_bomb() {
    int x, y;
    int* x_and_y = (int*)malloc(sizeof(int) * 2);
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
    printf("Bombing (%d,%d) awaiting success report..\n", x,y);
    x_and_y[0] = x;
    x_and_y[1] = y; 
    return x_and_y;
}
