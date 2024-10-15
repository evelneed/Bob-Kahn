#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "battleship.h"
#include "network_layer.h"

extern void startNetwork(int game_mode);
extern void start_screen();

int main() {
    printf("welcome to the network \n");
    printf("we have two applications, enter 1 to play battleship or 2 to use our chat applicatiion \n");
    printf("In the current state of our program the other machine needs to be running the same application simultaneously in order to function \n");

/*
    int* x;
    while (1) {
        printf("Enter desired application 1 or 2: ");
        fgets(*x, sizeof(int), stdin);
        if ((*x == 1 ) || (*x == 2)) {
            break;  // Valid input, exit the loop
        } else {
            printf("Invalid Input Try Again.\n");
        }
    }
	*/
    startNetwork(1);
	printf("start \n");
	start_screen();
   // if(*x ==1 ) {
   //     start_screen();
   // }
}
