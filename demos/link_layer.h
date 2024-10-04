#ifndef LINK_LAYER_H
#define LINK_LAYER_H
#include <stdint.h>

// Declare any global structures or variables here
struct g_variables {
    uint32_t previous_tick;
    uint32_t base_time;
    uint32_t rising_base_time;
    uint32_t falling_base_time;
    int first_falling_edge;
    int first_rising_edge;
    int not_first;
    int w;
    int i;
    int results[1000];
    int results_size;
    int message_done;
    int delay;
};
extern struct g_variables globals;

// Declare functions that will be used in network layer
void sendPulses(int bits[], int length);
void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick);
int* charToBit(char bits[], int length);
void send_header(int input[]);
void reset_variables();
void process_results();
void receivePacket();

#endif
