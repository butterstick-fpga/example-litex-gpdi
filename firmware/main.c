/*
 * Copyright 2021 Gregory Davill <greg.davill@gmail.com> 
 */

#include <generated/csr.h>
#include <generated/git.h>
#include <generated/mem.h>
#include <irq.h>
#include <sleep.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <uart.h>

#include <liblitedram/sdram.h>

void isr(void){

}

void led_blinking_task(void);

int main(int i, char **c) {
    irq_setie(0);

    vccio_ch0_write(6500);  // 3v3
    vccio_ch1_write(6500);  // 3v3
    vccio_ch2_write(6500);  // 3v3
    msleep(10);
    vccio_enable_write(1);


    while(!sdram_init());

	video_framebuffer_dma_base_write(MAIN_RAM_BASE);

    video_framebuffer_vtg_enable_write(0);
    video_framebuffer_dma_enable_write(0);

	memcpy(MAIN_RAM_BASE, SPIFLASH_BASE + 0x400000, video_framebuffer_dma_length_read());

    video_framebuffer_dma_enable_write(1);
    video_framebuffer_vtg_enable_write(1);

    while (1) {
	    led_blinking_task();
		busy_wait(50);

        if ((button_in_read() & 1) == 0){
			reset_out_write(1);
        }
    }

    return 0;
}

const uint16_t sine_pulse_fade[] = {0x000, 0x000, 0x000, 0x017, 0x04d, 0x09a, 0x0f4, 0x15a, 0x1c5, 0x235, 0x29f, 0x2fd, 0x354, 0x39a, 0x3cf, 0x3f1, 0x3ff, 0x3f1, 0x3cf, 0x39a, 0x354, 0x2fd, 0x29f, 0x235, 0x1c5, 0x15a, 0x0f4, 0x09a, 0x04d, 0x017, 0x000, 0x000};

void led_blinking_task(void) {
    static uint32_t start_ms = 0;
    static int count;

    count++;

    volatile uint32_t *p = (uint32_t *)CSR_LEDS_OUT0_ADDR;
    for (int i = 0; i < 7; i++) {
        p[i] = (uint32_t)sine_pulse_fade[(count + i * 2) % 32] << 10;  // Blue
    }
}
