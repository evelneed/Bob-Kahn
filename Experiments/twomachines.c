#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>
void main() {
	
	pigpio_start(0,0);
	//get user input into an array of bits to send
	char userInput[4];
	//ask the user for input
//	printf("please enter a sequence of 4 bits \n");
	//collect input
//	scanf("%s", userInput);
	for (int i = 0; i < 250; i++){
		/*
		//check if its a 1 or 0
		if (userInput[i] != '0' && userInput[i] != '1') {
			printf("invalid input please only bits, run the program and try again \n");
			exit(1);
		}*/
		//convert the char to the 
		//int N = userInput[i] - '0';
		//write it to the port 1 transmitter
		gpio_write(0, 27, 1);
		gpio_write(0, 27, 0);
		gpio_write(0, 27, 1);
		gpio_write(0, 27, 0);
	}
}	 

