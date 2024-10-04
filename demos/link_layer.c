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


int* charToBit(char bits[], int length) {
    int* myBits = (int*)malloc(length * sizeof(int) * 8);
    for (size_t i = 0; i < length; i++) {
        int asciiVal = (int)bits[i];
        for (int x = 8; x >= 0; x--) {
            myBits[i * 8 + x] = asciiVal % 2;
            int myNum = asciiVal % 2;
            printf("%d", myNum);
            asciiVal /= 2;
        }
    }
    return myBits;
}

void sendPulses(int bits[], int length) {
    int gpio = 27; // hardcoded to send from 27
    int state = 0; // starting in low
    for (int bitNum = 0; bitNum < length; bitNum++) {
        if (bits[bitNum] == 0 && state == 0) {
            gpio_write(0, gpio, 1);
            state = 1;
            usleep((bits[bitNum + 1] == 0) ? (globals.delay/2) : globals.delay);
        } else if (bits[bitNum] == 0 && state == 1) {
            gpio_write(0, gpio, 0);
            state = 0;
            bitNum--;
            usleep((globals.delay / 2));
        } else if (bits[bitNum] == 1 && state == 1) {
            gpio_write(0, gpio, 0);
            state = 0;
            usleep((bits[bitNum + 1] == 1) ? (globals.delay / 2) : globals.delay);
        } else if (bits[bitNum] == 1 && state == 0) {
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
            printf("End of message\n");
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

    printf("Message received: \n");
    for (int j = 0; j < globals.results_size; j++) {
        printf("%d,", globals.results[j]);
    }
    globals.message_done = 1;
    return globals.results;
}
