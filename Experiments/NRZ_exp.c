#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>

void main() {
	
	pigpio_start(0,0);
	//get user input into an array of bits to send
	int send[4];
	int recv[4];
	char userInput[4];
	//ask the user for input
	printf("please enter a sequence of 4 bits \n");
	//collect input
	scanf("%s", userInput);
	for (int i = 0; i <  4; i++){
		
		//check if its a 1 or 0
		if (userInput[i] != '0' && userInput[i] != '1') {
			printf("invalid input please only bits, run the program and try again \n");
			exit(1);
		}
		//convert the char to the int
		int N = userInput[i] - '0';
		//write it to the port 1 transmitter
		gpio_write(0, 27, N);
		sleep(0.0000001);
		//receieve and record the input in the port 4 receiver
		recv[i] = gpio_read(0, 20);
	}
	//read through and print the inputed bits
	printf("recieved sequence: ");
	for (int c= 0; c < 4; c++){
		printf("%d", recv[c]);
	}
}	 

