#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "link_layer.h"  // Include the header for the link layer

//get functions from the link_layer
extern void sendPulses(int bits[], int length);
extern void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick);
extern int* charToBit(char bits[], int length);
extern void process_results();
extern void reset_variables();
extern void send_header(int bits[]);
extern char* binary_to_char(int* results, int length);
extern my_input promptUser();


void main() {
    pigpio_start(0, 0);
    set_mode(0, 27, PI_OUTPUT);
    set_mode(0, 22, PI_INPUT);

    // Start receiving via callback
    int call_back_one = callback(0, 22, EITHER_EDGE, call_back);
    int set_times[4] = {1, 0, 1, 0}; // the header


    while (1) {
        if (globals.message_done) {
            globals.message_done = 0;
                // Limit input to 99 characters and ensure no buffer overflow
            my_input message = promptUser();
            send_header(message.input);
            sendPulses(message.input, message.length);
            time_sleep(1);

            // Reset state for the next message
            reset_variables();

            free(message.input);
            time_sleep(1);
        }
    }
}


