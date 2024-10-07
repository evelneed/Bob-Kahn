#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Structure to store global variables
struct g_variables {
    uint32_t previous_tick; // stores previous tick to be compared to current tick
    uint32_t base_time; // stores base time depending on state to compare to current time
    uint32_t rising_base_time; // stores rising edge base time
    uint32_t falling_base_time; // stores falling edge base time
    int first_falling_edge; // checks for first falling edge 
    int first_rising_edge; // checks for first rising edge
    int not_first; // checks to see if its the first state change
    int w; // counts transitions 
    int i;
    int results[1000];   // Dynamically allocated for received data
    int results_size; // size of results storage message for message
    int message_done; // checks to see if the message is received 
    const int delay;
};

typedef struct{
    int* input;
    int length;
} my_input;

// Initialize all the struct variables globally
struct g_variables globals = {
    .previous_tick = 0,
    .base_time = 0,
    .rising_base_time = 0,
    .falling_base_time = 0,
    .first_falling_edge = 1,
    .first_rising_edge = 1,
    .not_first = 1,
    .w = 0,
    .i = 0,
    .results_size = 0,
    .message_done = 1,
    .delay = 0.01 * 1000000,
};

void sendPulses(int bits[], int length);
void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick);
int* charToBit(char bits[], int length);
int* process_results();
void reset_variables();
void send_header(int bits[]);
my_input promptUser();
void char_to_binary(char c, int* bits);
char* binary_to_char(int* results, int length);


void reset_variables(){
    globals.results_size = 0;
    globals.previous_tick = 0;
    globals.base_time = 0;
    globals.rising_base_time = 0;
    globals.falling_base_time = 0;
    globals.first_falling_edge = 1;
    globals.first_rising_edge = 1;
    globals.not_first = 1;
}

my_input promptUser(){
    my_input message;
    // Limit input to 99 characters and ensure no buffer overflow
    char buffer[100];  
    printf("Please enter a message:\n");
    fgets(buffer, sizeof(buffer), stdin);  // Use fgets to safely get input
    size_t input_length = strlen(buffer);
    // Remove newline character from fgets input
    if (buffer[input_length - 1] == '\n') {
        buffer[input_length - 1] = '\0';
                input_length--;
            }
    // Allocate memory for input_char and input arrays
    message.length = input_length * 8; //adjust the message length based on conversion of chars to bits
    int *input = (int *)malloc((message.length) * sizeof(int));

    for (int i = 0; i < input_length; i++) {
        char_to_binary(buffer[i], &input[i * 8]);  // Convert character to binary
    }
    message.input = input;
    /*
    for (int i = 0; i < message.length; i++) {
	printf("%d",message.input[i]);
	fflush(stdout);
    }
    */
    return message;
}


void char_to_binary(char c, int* bits) {
    for (int i = 7; i >= 0; i--) {
        bits[7 - i] = (c >> i) & 1; // Extract individual bits
    }
}

char* binary_to_char(int* results, int length) {
    // make sure the string is a multiple of 8
    if (length % 8 != 0) {
        printf("Error: Bit length is not a multiple of 8 \n");
    }

    // Allocate memory for the result string
    int char_count = length / 8;
    char* finished_result = (char*)malloc((char_count + 1) * sizeof(char)); 

    for (int i = 0; i < char_count; i++) {
        char value = 0;
        for (int j = 0; j < 8; j++) {
            // Build the char by shifting and adding the current bit
            value = (value << 1) | results[i * 8 + j];
        }
        finished_result[i] = value;
    }
    return (finished_result);
}


//send header 
void send_header(int input[]){
	int set_times[4] = {1,0,1,0};
    for (int i = 0; i < 4; i++) { // send the header
        gpio_write(0, 27, set_times[i]);
        if (i == 3) {
            if (input[0] == 0) {
                usleep(globals.delay);
            }
            if (input[0] == 1) {
                usleep(globals.delay/2);
            }
            } else {
                usleep(globals.delay);
            }
    }
}



void sendPulses(int bits[], int length) {
    int gpio = 27; // hardcoded to send from 27
    int state = 0; // starting in low
    for (int bitNum = 0; bitNum < length; bitNum++) {
        if (bits[bitNum] == 0 && state == 0) {
            gpio_write(0, gpio, 1);
            state = 1;
	    if ((bitNum < length-1) && (bits[bitNum + 1] == 0)){
		    usleep(globals.delay /2);
	   } else {
		   usleep(globals.delay);
        } }else if (bits[bitNum] == 0 && state == 1) {
            gpio_write(0, gpio, 0);
            state = 0;
            bitNum--;
            usleep((globals.delay / 2));
        } else if (bits[bitNum] == 1 && state == 1) {
            gpio_write(0, gpio, 0);
            state = 0;
            if ((bitNum < length - 1)&& bits[bitNum + 1] == 1){
		    usleep(globals.delay/2);
		} else {
		usleep(globals.delay);
        } }
	else if (bits[bitNum] == 1 && state == 0) {
            gpio_write(0, gpio, 1);
            state = 1;
            bitNum--;
            usleep((globals.delay / 2));
        }
    }
    usleep(globals.delay * 3.5);
    gpio_write(0, 27, (bits[length - 1] == 0) ? 0 : 1);
}

// Callback for receiving functionality
void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick) {
    if (globals.rising_base_time == 0 && globals.first_rising_edge == 0 && level == 0) {
        globals.rising_base_time = tick - globals.previous_tick;
        globals.not_first = 0;
    }

    if (globals.first_falling_edge && level == 1 && globals.first_rising_edge == 0) {
        globals.falling_base_time = tick - globals.previous_tick;
        globals.first_falling_edge = 0;
        globals.not_first = 0;
    }

    if (globals.first_rising_edge && level == 0) {
        globals.previous_tick = tick;
        globals.first_rising_edge = 0;
    }

    globals.base_time = (level == 0) ? globals.rising_base_time : globals.falling_base_time;

    uint32_t current_time = tick - globals.previous_tick;
    double percent_error = fabs(((double)globals.base_time - current_time) / globals.base_time) * 100.0;

    if (globals.previous_tick != 0 && globals.base_time != 0 && globals.not_first == 0) {
        if (percent_error > 300) {
            process_results();
        } else if (percent_error > 25) {
            // Handle incorrect transition if needed
        } else {
            globals.previous_tick = tick;
            if (globals.results_size < 1000) {
                globals.results[globals.results_size] = level;
                globals.results_size++;
            } else {
                printf("Maximum bits reached\n");
            }
        }
    }
    globals.w++;
}

// Process received results
int* process_results() {
    if (globals.results_size < 2) {
        printf("Not enough bits to process\n");
        return 0;
    }

    for (int j = 2; j < globals.results_size; j++) {
        globals.results[j - 2] = globals.results[j];
    }

    globals.results_size -= 2;

    for (int j = 0; j < globals.results_size; j++) {
        globals.results[j] = globals.results[j] == 0 ? 1 : 0;
    }

    printf(" \n Message received: \n");
    for (int j = 0; j < globals.results_size; j++) {
        printf("%d,", globals.results[j]);
    }
    globals.message_done = 1;
	
    return globals.results;
}


