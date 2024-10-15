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
void* sendMessage(void* args);
void *receive(void* args);
void* process(void* args);
void redo_message(char senders_add);

pthread_mutex_t lock;

typedef struct {
    char name;
    char neighbors[4];
    int forward[4]; //which port to send to
    int end; //1 if it is an "end" machine in a chain so should prompt user
    char myDest; 
    int send_pins[4];
    int recv_pins[4]; //pins to send and receive, 1 off from their port # 
} machine;

typedef struct message{
    int* text;
    int len;
    struct message* next; //points to the next one
} message;

typedef struct Queue{
   message* front;
   message* end;
} Queue;


Queue* send_queue;

//this means that b communicates with a on port 1 and c on port 2
machine this_machine = { //global variable for this machine
    .name = 'c',
    .neighbors = {'b', '0', '0', '0'},
    .forward = {4, 0, 0, 0},
    .end = 1,
    .myDest = 'a', //COME HERE TO CHOOSE WHERE TO SEND TO
    .send_pins = {27, 25, 23, 21 },
    .recv_pins =  {26, 24, 22, 20},

}; //come here and change for each machine!!!


int main() {
    pigpio_start(0, 0);
    pthread_mutex_init(&lock, NULL);
    int i = 0;
    int port = this_machine.forward[i];
    while (port != 0){ //setting the send pins to pi_output, pi_output and makes calback on each recv pin, could clean up w for loop structure
        set_mode(0, this_machine.send_pins[port - 1], PI_OUTPUT);
        set_mode(0, this_machine.recv_pins[port - 1], PI_INPUT);
        callback(0, this_machine.recv_pins[port - 1], EITHER_EDGE, call_back);
        i++;
        port = this_machine.forward[i];
    }

send_queue = (Queue*)malloc(sizeof(Queue));
send_queue->front = NULL;
send_queue->end = NULL;

    int err;
    pthread_t userThread; //gets user input, can get blocked
    pthread_t sendThread; //sends user input
   pthread_t recvThread; //receives messages
   // pthread_t processThread;
    err = pthread_create(&userThread, NULL, &input, NULL);
    err = pthread_create(&recvThread, NULL, &receive, NULL);
    err = pthread_create(&sendThread, NULL, &sendMessage, NULL);
    err = pthread_join(recvThread, NULL);
    err = pthread_join(userThread, NULL);
    err = pthread_join(sendThread, NULL);
   // err = pthread_join(processThread, NULL);
    return 0;
}

void addToQueue(int* input, int length){
  //  printf("i am adding to queue \n");
    pthread_mutex_lock(&lock);
    message* new_message = (message*)malloc(sizeof(message));
    new_message->text = input;
    new_message->len = length;
    new_message->next = NULL;


    if (send_queue->end == NULL){ //if the queue is empty
        send_queue->front = send_queue->end = new_message;
    }
    else{
        send_queue->end->next = new_message;
        send_queue->end = new_message;
    }
    pthread_mutex_unlock(&lock);
}


message* dequeue(){
	while (send_queue->front != NULL){
		pthread_mutex_lock(&lock);
		message* toSend = send_queue->front;
		send_queue->front = send_queue->front->next;
		if (send_queue->front == NULL){
			send_queue->end = NULL;
		}
		pthread_mutex_unlock(&lock);
		return toSend;
	}
	return 0;
}

void* sendMessage(void* args){
	while(1) {
	   message* toSend = dequeue();
	   if (toSend == 0){
		   continue;
	   }
        int gpio = this_machine.send_pins[getPath(this_machine.myDest) -1];
        sendPulses(toSend->text, toSend->len, gpio); //sends their message
	//move the head over
	free(toSend->text);
	free(toSend);
    }
}
void* input(void* args){ //this thread can get blocked
    while (1){
         if (this_machine.end){ //if this is the first or last
         	    my_input newInput = promptUser(this_machine.name, this_machine.myDest);   
		        addToQueue( newInput.input, newInput.length);
		        usleep(1000);
		       // free(newInput.input);
		
            }
    }
    usleep(1000);
}

void* receive(void* args){
    while (1){
         if (globals.message_received) { //don't need to change it back to zero cause that happens in reset variables, this could be bad.....
		 if(globals.results_size % 8 > 2){ //checks if there was an error sending a message and tells the sender to send it again
		    //must get the address of the sender 
		    int* message_int = globals.results;
		    int length = globals.results_size;
		    char* message = binary_to_char(message_int, 8);
                    redo_message(message[0]);
		//    free(message); 
                    reset_variables();
                    continue;
                } 
		else{
                int* message_int = globals.results; 
                int length = globals.results_size;
		char* message = binary_to_char(message_int, length);
                int check = check_dest(message);
                if (check) { //if this is my final destination, print it out
                    printf("Message received: %s \n", message + 2); 
                    fflush(stdout);  
                    // Reset state for the next message
                    reset_variables();              
                }
                else { //must forward message, but to where??
			        int* input = (int*)malloc((length + 4) * sizeof(int));
			        input[0] = 0;
			        input[1] = 1;
			        input[2] = 0;
			        input[3] = 1;
			        for (int i = 0; i < length; i++){
				        input[i + 4] = message_int[i];
			        }
			        //change the destination
			        this_machine.myDest = message[1];		
                    addToQueue(input, length+4); //I have updated my destination so this should be fine
                }
                usleep(1000);
                free(message);
                reset_variables();
		}
	    }
    }
}

void redo_message(char senders_add){ //sends a message to the sender to send their message again
	usleep(1000);
	my_input redo_message_final;
	char* redo = "error transmitting message, please send again";
//	printf("this machines name: %c sending to: %c \n", this_machine.name, senders_add);
    char* redo_message = addMessage(redo, 45, this_machine.name, senders_add);
    redo_message_final.length = (47* 8) + 4; //allocating length for the bits and intr0
    int *input = (int *)malloc((redo_message_final.length) * sizeof(int));
    input[0] = 0;
    input[1] = 1;
    input[2] = 0;
    input[3] = 1;
    for (int i = 0; i < 47; i++){
	char_to_binary(redo_message[i], &input[(i * 8) + 4]); 
    }
    redo_message_final.input = input;
    this_machine.myDest = senders_add;
 //   send_header(redo_message_final.input, gpio);
  //  sendPulses(redo_message_final.input, redo_message_final.length, gpio); //sends their message!
	addToQueue(redo_message_final.input, redo_message_final.length);
    	usleep(1000);
	free(redo_message);
    //	free(input);
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
