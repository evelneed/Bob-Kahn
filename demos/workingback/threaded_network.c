#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "link_layer.h"  // Include the header for the link layer
#include <pthread.h>

//get functions from the link_layer
extern void sendPulses(int bits[], int length, int gpio);
extern void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick);
extern char* addMessage(char* message, int length, char home, char dest);
extern void process_results();
extern void reset_variables();
extern void send_header(int bits[], int gpio);
extern my_input promptUser(char home, char dest);
extern void char_to_binary(char c, int* bits);
extern char* binary_to_char(int* results, int length);

//this classes functions
int check_dest(char* message);
int getPath(char dest);
void* input(void* args);
void* process(void* args);
void redo_message(char senders_add);

typedef struct {
    char name;
    char neighbors[4];
    int forward[4]; //which port to send to
    int end; //1 if it is an "end" machine in a chain so should prompt user
    char myDest; 
    int send_pins[4];
    int recv_pins[4]; //pins to send and receive, 1 off from their port # 
} machine;

//this means that b communicates with a on port 1 and c on port 2
machine this_machine = { //global variable for this machine
    .name = 'b',
    .neighbors = {'a', 'c', '0', '0'},
    .forward = {3, 4, 0, 0},
    .end = 0,
    .myDest = 'a', //COME HERE TO CHOOSE WHERE TO SEND TO
    .send_pins = {27, 25, 23, 21 },
    .recv_pins =  {26, 24, 22, 20},

}; //come here and change for each machine!!!

int main() {
    pigpio_start(0, 0);

    int i = 0;
    int port = this_machine.forward[i];
    while (port != 0){ //setting the send pins to pi_output, pi_output and makes calback on each recv pin, could clean up w for loop structure
        set_mode(0, this_machine.send_pins[port - 1], PI_OUTPUT);
        set_mode(0, this_machine.recv_pins[port - 1], PI_INPUT);
        callback(0, this_machine.recv_pins[port - 1], EITHER_EDGE, call_back);
        i++;
        port = this_machine.forward[i];
    }

    int err;
    pthread_t userThread;
    pthread_t processThread;
    err = pthread_create(&userThread, NULL, &input, NULL);
    err = pthread_create(&processThread, NULL, &process, NULL);
    err = pthread_join(userThread, NULL);
    err = pthread_join(processThread, NULL);
    return 0;
}

void* input(void* args){ //this thread can get blocked
    while (1){
         if (this_machine.end){ //if this is the first or last
                my_input newInput = promptUser(this_machine.name, this_machine.myDest);
                int gpio = this_machine.send_pins[getPath(this_machine.myDest) - 1];
                send_header(newInput.input, gpio);
                sendPulses(newInput.input, newInput.length, gpio); //sends their message!
		        sleep(.001);
		        free(newInput.input);
            }
    }
    time_sleep(1);
}

int b = 0;
void* process(void* args){
    while (1){
         if (globals.message_received) { //don't need to change it back to zero cause that happens in reset variables, this could be bad.....
                if( globals.message_error == 1 || globals.results_size % 8 != 0 ){ //checks if there was an error sending a message and tells the sender to send it again
		    printf("\n error detected \n");
		    //must get the address of the sender 
		    int* message_int = globals.results;
		    int length = globals.results_size;
		    char* message = binary_to_char(message_int, 10); //to convert the message to get address
            //TODO make conversion work becuase when the message is transmitted it doesnt inlcude the whole thing
                    redo_message(message[0]); 
                    reset_variables();
                    continue;
                }
		else{	
                int* message_int = globals.results; //come back here and change in link
                int length = globals.results_size;
                char* message = binary_to_char(message_int, length); //probably have to change the name, wait for oliver's ascii
                int check = check_dest(message);
                if (check) { //if this is my final destination, print it out
                    printf("Message received: %s \n", message + 2); //start at 2 after the addressing info
                    fflush(stdout);  
                    // Reset state for the next message
                    reset_variables();              
                }
                else { //must forward message, but to where??
                    int gpio = this_machine.send_pins[getPath(message[1]) - 1];
                    send_header(message_int, gpio);
                    sendPulses(message_int, length, gpio);
		    reset_variables(); 
                }
                free(message);
                time_sleep(1);
		}
	    }
    }
}

void redo_message(char senders_add){ //sends a message to the sender to send their message again
	sleep(1);
	printf("in redo message \n");
	my_input redo_message_final;
	char* redo = "error transmitting message, please send again";
	printf("this machines name: %c sending to: %c \n", this_machine.name, senders_add); 
    //creates the message with the device that caught the error as the home and the sender as address
    char* redo_message = addMessage(redo, 45, this_machine.name, senders_add);
    redo_message_final.length = 47* 8 ; //allocating length for the bits and intr0
    int *input = (int *)malloc((redo_message_final.length) * sizeof(int));
    for (int i = 0; i < 47; i++){
	char_to_binary(redo_message[i], &input[i * 8]); //convert message to binary and send
    }
    redo_message_final.input = input;
   // int gpio = this_machine.send_pins[getPath(this_machine.myDest)-1];
    int gpio = 21;
    printf("gpio: %d \n", gpio);
    send_header(redo_message_final.input, gpio);
    sendPulses(redo_message_final.input, redo_message_final.length, gpio); //sends their message!
	sleep(.001);
	free(redo_message);
    	free(input);
}

int getPath(char dest){ //gets the path to get to the given destination, returns which port number to send to
    if (this_machine.neighbors[1] == '0'){ //if there is only one neighbor
        return this_machine.forward[0];
    }
    else {
        for (int i = 0; i < 4; i++){
            if (this_machine.neighbors[i] == dest){
                return this_machine.forward[i]; //this works because the indices are aligned
            }
        }
    }
}

int check_dest(char* message){
    char dest = message[1];
    if (dest == this_machine.name ) {
        return 1;
    }
    return 0;
}
