/*
 * Copyright 2021 Gregory Davill <greg.davill@gmail.com> 
 */

#include <stdlib.h>
#include <stdio.h>

#include <generated/csr.h>
#include <generated/mem.h>
#include <generated/git.h>
#include <generated/luna_usb.h>

#include <irq.h>
#include <uart.h>

#include <sleep.h>

#include "tusb.h"

// Current system tick timer.
volatile uint32_t system_ticks = 0;

volatile uint32_t reset_countdown = 0;

#if CFG_TUSB_OS == OPT_OS_NONE
uint32_t board_millis(void)
{
	return system_ticks;
}
#endif

//--------------------------------------------------------------------+
// Board porting API
//--------------------------------------------------------------------+

static void timer_init(void)
{
	// Set up our timer to generate an interrupt every millisecond.
	timer0_reload_write(60 * 1000);
	timer0_en_write(1);
	timer0_ev_enable_write(1);

	// Enable our timer's interrupt.
	irq_setie(1);
	irq_setmask((1 << TIMER0_INTERRUPT) | irq_getmask());
}

static void timer_isr(void)
{
	// Increment our total millisecond count.
	++system_ticks;
}

void isr(void)
{
	unsigned int irqs;
	irqs = irq_pending() & irq_getmask();

	// Dispatch USB events.
	if (irqs & (1 << USB_DEVICE_CONTROLLER_INTERRUPT | 1 << USB_IN_EP_INTERRUPT | 1 << USB_OUT_EP_INTERRUPT | 1 << USB_SETUP_INTERRUPT))
	{
		tud_int_handler(0);
	}

	// Dispatch timer events.
	if (irqs & (1 << TIMER0_INTERRUPT))
	{
		timer0_ev_pending_write(timer0_ev_pending_read());
		timer_isr();
	}

	// Dispatch UART events.
	if (irqs & (1 << UART_INTERRUPT))
	{
		uart_isr();
	}
}

int main(int i, char **c)
{

	/* Setup IRQ, needed for UART */
	irq_setmask(0);
	irq_setie(1);
	uart_init();

	vccio_ch0_write(45000); // 1v8
	vccio_ch1_write(45000); // 1v8
	vccio_ch2_write(45000); // 1v8
	msleep(10);
	vccio_enable_write(1);

	usb_device_controller_reset_write(1);
	msleep(20);
	usb_device_controller_reset_write(0);
	msleep(20);

	timer_init();
	tusb_init();

	while (1)
	{
		tud_task(); // tinyusb device task
		led_blinking_task();

		if (reset_countdown > 0)
		{
			reset_countdown--;
			if (reset_countdown == 0)
			{
				reset_out_write(1);
			}
		}
	}

	return 0;
}

const uint16_t sine_pulse_fade[] = {0x000, 0x000, 0x000, 0x017, 0x04d, 0x09a, 0x0f4, 0x15a, 0x1c5, 0x235, 0x29f, 0x2fd, 0x354, 0x39a, 0x3cf, 0x3f1, 0x3ff, 0x3f1, 0x3cf, 0x39a, 0x354, 0x2fd, 0x29f, 0x235, 0x1c5, 0x15a, 0x0f4, 0x09a, 0x04d, 0x017, 0x000, 0x000};

void led_blinking_task(void)
{
	static uint32_t start_ms = 0;
	static int count;

	// Blink every interval ms
	if ((board_millis() - start_ms) < 40)
		return; // not enough time
	start_ms += 40;
	count++;

	volatile uint32_t *p = (uint32_t *)CSR_LEDS_OUT0_ADDR;
	for (int i = 0; i < 7; i++)
	{
		p[i] = (uint32_t)sine_pulse_fade[(count + i * 2) % 32] << 10; // Blue
	}
}

// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
	(void)remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

// Invoked on DFU_DETACH request to reboot to the bootloader
void tud_dfu_runtime_reboot_to_dfu_cb(void)
{


	uint8_t buf[256] = {0};
	/* Check for magic bytes in the Security page3 */
	const uint32_t BL_MAGIC0 = 0x021b3bcd;
	const uint32_t BL_MAGIC1 = 0xc4f86d8a;

	buf[0] = (BL_MAGIC0 >> 0) & 0xFF;
	buf[1] = (BL_MAGIC0 >> 8) & 0xFF;
	buf[2] = (BL_MAGIC0 >> 16) & 0xFF;
	buf[3] = (BL_MAGIC0 >> 24) & 0xFF;

	spiflash_write_enable();
	spiflash_erase_security_register(3);
	
	spiflash_write_enable();
	spiflash_write_security_register(3, buf);

	reset_countdown = 10000;
}