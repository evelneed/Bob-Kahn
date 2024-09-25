#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>

void main() {
	pigpio_start(0,0);
	printf("%d \n", wave_tx_busy(0));
	wave_clear(0);
	gpioPulse_t pulses[2];
	gpio_write(0, 25,0);
	gpio_write(0, 27, 0);
	//make pulses
	pulses[0].gpioOn = 1<< 25;
	pulses[0].gpioOff = 1 << 27;
	pulses[0].usDelay = 40;
	pulses[1].gpioOn = 1<< 27;
	pulses[1].gpioOff = 1 << 25;
	pulses[1].usDelay = 40;
	wave_add_generic(0, 2, pulses);
	int wave_id = wave_create(0);
	int send = wave_send_once(0, wave_id);
	printf("%d \n", gpio_read(0, 27));
	printf("%d \n", pulses[0].gpioOn);
	gpio_trigger(0, 27, 5000, 1);
//	wave_tx_stop(0);
}

