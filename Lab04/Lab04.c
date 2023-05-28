/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2014 Chuck McManis <cmcmanis@mcmanis.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <setjmp.h>
#include <unistd.h>
#include <string.h>

#include "clock.h"
#include "console.h"
#include "sdram.h"
#include "lcd-spi.h"
#include "gfx.h"
#include "clock.h"


#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>



#define L3GD20_SENSITIVITY_250DPS  (0.00875F)  

//-----------------------------------------------

#define CONSOLE_UART	USART1



/* this is for fun, if you type ^C to this example it will reset */
#define RESET_ON_CTRLC

#ifdef RESET_ON_CTRLC

/* Jump buffer for setjmp/longjmp */
jmp_buf	jump_buf;

static void do_the_nasty(void);
/*
 * do_the_nasty
 *
 * This is a hack to implement the equivalent of a signal interrupt
 * in a system without signals or a kernel or scheduler. Basically
 * when the console_getc() function reads a '^C' character, it munges
 * the return address of the interrupt to here, and then this function
 * does a longjump to the last place we did a setjmp.
 */
static void do_the_nasty(void)
{
	longjmp(jump_buf, 1);
	while (1);
}
#endif

/* This is a ring buffer to holding characters as they are typed
 * it maintains both the place to put the next character received
 * from the UART, and the place where the last character was
 * read by the program. See the README file for a discussion of
 * the failure semantics.
 */
#define RECV_BUF_SIZE	128		/* Arbitrary buffer size */

		/* Next place to read */

/* For interrupt handling we add a new function which is called
 * when recieve interrupts happen. The name (usart1_isr) is created
 * by the irq.json file in libopencm3 calling this interrupt for
 * USART1 'usart1', adding the suffix '_isr', and then weakly binding
 * it to the 'do nothing' interrupt function in vec.c.
 *
 * By defining it in this file the linker will override that weak
 * binding and instead bind it here, but you have to get the name
 * right or it won't work. And you'll wonder where your interrupts
 * are going.
 */


/*
 * console_putc(char c)
 *
 * Send the character 'c' to the USART, wait for the USART
 * transmit buffer to be empty first.
 */


/*
 * char = console_getc(int wait)
 *
 * Check the console for a character. If the wait flag is
 * non-zero. Continue checking until a character is received
 * otherwise return 0 if called and no character was available.
 *
 * The implementation is a bit different however, now it looks
 * in the ring buffer to see if a character has arrived.
 */

/*
 * void console_puts(char *s)
 *
 * Send a string to the console, one character at a time, return
 * after the last character, as indicated by a NUL character, is
 * reached.
 */


/*
 * int console_gets(char *s, int len)
 *
 * Wait for a string to be entered on the console, limited
 * support for editing characters (back space and delete)
 * end when a <CR> character is received.
 */


void countdown(void);

/*
 * countdown
 *
 * Count down for 20 seconds to 0.
 *
 * This provides an example function which is constantly
 * printing for 20 seconds and not looking for typed characters.
 * however with the interrupt driven receieve queue you can type
 * ^C while it is counting down and it will be interrupted.
 */
void countdown(void)
{
	int i = 200;
	while (i-- > 0) {
		console_puts("Countdown: ");
		console_putc((i / 600) + '0');
		console_putc(':');
		console_putc(((i % 600) / 100) + '0');
		console_putc((((i % 600) / 10) % 10) + '0');
		console_putc('.');
		console_putc(((i % 600) % 10) + '0');
		msleep(100);
	}
}


//-----------------------------------------------





/*
 * Functions defined for accessing the SPI port 8 bits at a time
 */
uint16_t read_reg(int reg);
void write_reg(uint8_t reg, uint8_t value);
uint8_t read_xyz(int16_t vecs[3]);
void spi_init(void);
void put_status(char *m);
/* Convert degrees to radians */
#define d2r(d) ((d) * 6.2831853 / 360.0)



//-----------------------------------------------
void put_status(char *m)
{	
	//--------------
	char buf[128];
	int	len;
	bool pmask;
	//--------------

	uint16_t stmp;

	console_puts(m);
	console_puts(" Status: ");
	stmp = SPI_SR(SPI5);
	if (stmp & SPI_SR_TXE) {
		console_puts("TXE, ");
	}
	if (stmp & SPI_SR_RXNE) {
		console_puts("RXNE, ");
	}
	if (stmp & SPI_SR_BSY) {
		console_puts("BUSY, ");
	}
	if (stmp & SPI_SR_OVR) {
		console_puts("OVERRUN, ");
	}
	if (stmp & SPI_SR_MODF) {
		console_puts("MODE FAULT, ");
	}
	if (stmp & SPI_SR_CRCERR) {
		console_puts("CRC, ");
	}
	if (stmp & SPI_SR_UDR) {
		console_puts("UNDERRUN, ");
	}
	console_puts("\n");
}

uint16_t
read_reg(int reg)
{
	uint16_t d1, d2;

	d1 = 0x80 | (reg & 0x3f); /* Read operation */
	/* Nominallly a register read is a 16 bit operation */
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, d1);
	d2 = spi_read(SPI5);
	d2 <<= 8;
	/*
	 * You have to send as many bits as you want to read
	 * so we send another 8 bits to get the rest of the
	 * register.
	 */
	spi_send(SPI5, 0);
	d2 |= spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);
	return d2;
}

uint8_t
read_xyz(int16_t vecs[3])
{	
	
	uint8_t	 buf[7];
	int		 i;

	gpio_clear(GPIOC, GPIO1); /* CS* select */
	spi_send(SPI5, 0xc0 | 0x28);
	(void) spi_read(SPI5);
	for (i = 0; i < 6; i++) {
		spi_send(SPI5, 0);
		buf[i] = spi_read(SPI5);
	}
	gpio_set(GPIOC, GPIO1); /* CS* deselect */
	vecs[0] = (buf[1] << 8 | buf[0]);
	vecs[1] = (buf[3] << 8 | buf[2]);
	vecs[2] = (buf[5] << 8 | buf[4]);
	

	return read_reg(0x27); /* Status register */
}

void
write_reg(uint8_t reg, uint8_t value)
{
	gpio_clear(GPIOC, GPIO1); /* CS* select */
	spi_send(SPI5, reg);
	(void) spi_read(SPI5);
	spi_send(SPI5, value);
	(void) spi_read(SPI5);
	gpio_set(GPIOC, GPIO1); /* CS* deselect */
	return;
}

int print_decimal(int);

/*
 * int len = print_decimal(int value)
 *
 * Very simple routine to print an integer as a decimal
 * number on the console.
 */
int
print_decimal(int num)
{
	int		ndx = 0;
	char	buf[10];
	int		len = 0;
	char	is_signed = 0;

	if (num < 0) {
		is_signed++;
		num = 0 - num;
	}
	buf[ndx++] = '\000';
	do {
		buf[ndx++] = (num % 10) + '0';
		num = num / 10;
	} while (num != 0);
	ndx--;
	if (is_signed != 0) {
		console_putc('-');
		len++;
	}
	while (buf[ndx] != '\000') {
		console_putc(buf[ndx--]);
		len++;
	}
	return len; /* number of characters printed */
}

char *axes[] = { "X: ", "Y: ", "Z: " };

//-----------------------------------------------

static void button_setup(void)
{
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Set GPIO0 (in GPIO port A) to 'input open-drain'. */
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
}

static void gpio_setup(void)
{
	/* Enable GPIOG clock. */
	rcc_periph_clock_enable(RCC_GPIOG);

	/* Set GPIO13 (in GPIO port G) to 'output push-pull'. */
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT,
			GPIO_PUPD_NONE, GPIO13);
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT,
			GPIO_PUPD_NONE, GPIO14);
}

//-----------------------------------------------

static void adc_setup(void)
{	
	rcc_periph_clock_enable(RCC_ADC1);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO2);
	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);
	adc_power_on(ADC1);

}


static uint16_t read_adc_naiive(uint8_t channel)
{
	uint8_t channel_array[16];
	channel_array[0] = channel;
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_start_conversion_regular(ADC1);
	while (!adc_eoc(ADC1));
	uint16_t reg16 = adc_read_regular(ADC1);
	return reg16;
}
	//-----------------------------------------------------------

/*
 * This is our example, the heavy lifing is actually in lcd-spi.c but
 * this drives that code.
 */
int main(void)
{


	button_setup();
	gpio_setup();
	clock_setup();
	console_setup(115200);
	sdram_init();
	lcd_spi_init();
	adc_setup();
	console_puts("LCD Initialized\n");
	console_puts("Should have a checker pattern, press any key to proceed\n");
	msleep(2000);
/*	(void) console_getc(1); */
	gfx_init(lcd_draw_pixel, 240, 320);
	gfx_fillScreen(LCD_GREY);
	gfx_fillRoundRect(10, 10, 220, 220, 5, LCD_WHITE);
	gfx_drawRoundRect(10, 10, 220, 220, 5, LCD_RED);
	gfx_fillCircle(20, 250, 10, LCD_RED);
	gfx_fillCircle(120, 250, 10, LCD_GREEN);
	gfx_fillCircle(220, 250, 10, LCD_BLUE);
	gfx_setTextSize(2);
	gfx_setCursor(15, 25);
	gfx_puts("Sismografo");
	gfx_setTextSize(1);
	gfx_setCursor(15, 49);
	gfx_puts("STM32: GPIO,");
	gfx_setCursor(15, 60);
	gfx_puts("ADC, comunicaciones,");
	gfx_setCursor(15, 71);
	gfx_puts("Iot");
	gfx_setCursor(15, 85);
	gfx_puts("Estudiante:");
	gfx_setCursor(15, 96);
	gfx_puts("Derian Monge Calvo");
	lcd_show_frame();
	console_puts("Now it has a bit of structured graphics.\n");
	console_puts("Press a key for some simple animation.\n");
	msleep(2000);
/*	(void) console_getc(1); */
	gfx_setTextColor(LCD_YELLOW, LCD_BLACK);
	gfx_setTextSize(3);
	//-----------------------------------------------------------------------

	char buf[128];
	int	len;
	bool pmask;
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9 | GPIO10);
	rcc_periph_clock_enable(RCC_USART1);

	usart_set_baudrate(CONSOLE_UART, 115200);
	usart_set_databits(CONSOLE_UART, 8);
	usart_set_stopbits(CONSOLE_UART, USART_STOPBITS_1);
	usart_set_mode(CONSOLE_UART, USART_MODE_TX_RX);
	usart_set_parity(CONSOLE_UART, USART_PARITY_NONE);
	usart_set_flow_control(CONSOLE_UART, USART_FLOWCONTROL_NONE);
	usart_enable(CONSOLE_UART);

	/* Enable interrupts from the USART */
	nvic_enable_irq(NVIC_USART1_IRQ);

	/* Specifically enable recieve interrupts */
	usart_enable_rx_interrupt(CONSOLE_UART);

		console_puts("\nUART Demonstration Application\n");
#ifdef RESET_ON_CTRLC
	console_puts("Press ^C at any time to reset system.\n");
	pmask = cm_mask_interrupts(0);
	cm_mask_interrupts(pmask);
	if (setjmp(jump_buf)) {
		console_puts("\nInterrupt received! Restarting from the top\n");
	}
#endif

	//-----------------------------------------------------------

	
	int16_t vecs[3];
	int16_t baseline[3];
	int tmp, i;
	int count;
	uint32_t cr_tmp;
	/* Enable the GPIO ports whose pins we are using */
	rcc_periph_clock_enable(RCC_GPIOF | RCC_GPIOC);

	gpio_mode_setup(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN,
			GPIO7 | GPIO8 | GPIO9);
	gpio_set_af(GPIOF, GPIO_AF5, GPIO7 | GPIO8 | GPIO9);
	gpio_set_output_options(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
				GPIO7 | GPIO9);

	/* Chip select line */
	gpio_set(GPIOC, GPIO1);
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);

	rcc_periph_clock_enable(RCC_SPI5);

	cr_tmp = SPI_CR1_BAUDRATE_FPCLK_DIV_8 |
		 SPI_CR1_MSTR |
		 SPI_CR1_SPE |
		 SPI_CR1_CPHA |
		 SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE;

	put_status("\nBefore init: ");
	SPI_CR2(SPI5) |= SPI_CR2_SSOE;
	SPI_CR1(SPI5) = cr_tmp;
	put_status("After init: ");


	baseline[0] = 0;
	baseline[1] = 0;
	baseline[2] = 0;
	console_puts("MEMS demo (new version):\n");
	console_puts("Press a key to read the registers\n");
	//console_getc(1);
	tmp = read_reg(0xf);
	if (tmp != 0xD4) {
		gfx_setCursor(15, 36);
		gfx_puts("error");
	}
	/*
	 * These parameters are sort of random, clearly I need
	 * set something. Based on the app note I reset the 'baseline'
	 * values after 100 samples. But don't see a lot of change
	 * when I move the board around. Z doesn't move at all but the
	 * temperature reading is correct and the ID code returned is
	 * as expected so the SPI code at least is working.
	 */
	write_reg(0x20, 0xcf);  /* Normal mode */
	write_reg(0x21, 0x07);  /* standard filters */
	write_reg(0x23, 0xb0);  /* 250 dps */
	tmp = (int) read_reg(0x26);
	

	count = 0;
	bool USART_mode = false; 

	//-----------------------------------------------------------------------
	while (1) {
		
		if (gpio_get(GPIOA, GPIO0)) {
			while (gpio_get(GPIOA, GPIO0))
			{
				__asm__("nop");
			}
			USART_mode = !USART_mode;
		}
		if(USART_mode == true){
			gpio_toggle(GPIOG, GPIO13);
		}
		else{
			gpio_clear(GPIOG, GPIO13);
			gfx_setTextSize(2);
			gfx_setCursor(100, 300);
			gfx_puts("USART OFF");
		}
		
		
		//-----------------------------
		tmp = read_xyz(vecs);
		for (i = 0; i < 3; i++) {
			int pad;
			
			tmp = vecs[i] - baseline[i];

		}
		
		if (count == 100) {
			baseline[0] = vecs[0];
			baseline[1] = vecs[1];
			baseline[2] = vecs[2];
		} else {
			count++;
		}
		for (i = 0; i < 80000; i++)    /* Wait a bit. */
			__asm__("nop");
		//-----------------------------
		char a[10];
		char b[10];
		char c[10];
		int x = baseline[0]*L3GD20_SENSITIVITY_250DPS;
		int y = baseline[1]*L3GD20_SENSITIVITY_250DPS;
		int z = baseline[2]*L3GD20_SENSITIVITY_250DPS;
		sprintf(a, "%d", x);
		sprintf(b, "%d", y);
		sprintf(c, "%d", z);

		char v[10];
		uint16_t input_adc1 = read_adc_naiive(2);
		sprintf(v, "%d", input_adc1);

		

		gfx_setTextColor(LCD_BLUE, LCD_BLACK);
		gfx_setTextSize(2);

		gfx_fillScreen(LCD_BLACK);
		gfx_setCursor(10, 36);
		gfx_puts("Sismografo UCR");

		gfx_setTextColor(LCD_YELLOW, LCD_BLACK);
		gfx_setTextSize(2);

		gfx_setCursor(90, 106);
		gfx_puts("x=");
		gfx_setCursor(130, 106);
		gfx_puts(a);
		gfx_setCursor(90, 146);
		gfx_puts("y=");
		gfx_setCursor(130, 146);
		gfx_puts(b);
		gfx_setCursor(90, 186);
		gfx_puts("z=");
		gfx_setCursor(130, 186);
		gfx_puts(c);
		gfx_setCursor(90, 226);
		gfx_setTextColor(LCD_GREEN, LCD_BLACK);

		gfx_puts("V=");
		gfx_setCursor(130, 226);
		gfx_puts(v);
		if(input_adc1<7){
			gfx_setTextColor(LCD_RED, LCD_BLACK);
			gfx_setCursor(10, 266);
			gpio_toggle(GPIOG, GPIO14);
			gfx_puts("Bateria baja");
		}
		else{
			gpio_clear(GPIOG, GPIO14);
		}

		gfx_setTextColor(LCD_MAGENTA, LCD_BLACK);
		if(USART_mode == true){
			gfx_setTextSize(2);
			gfx_setCursor(10, 300);
			gfx_puts("USART ON");
			print_decimal(x);
			console_puts(";");
			print_decimal(y);
			console_puts(";");
			print_decimal(z);
			console_puts(";");
			print_decimal(input_adc1);
			console_puts(";");
			console_puts(":");

		}
		else{
			gfx_setTextSize(2);
			gfx_setCursor(10, 300);
			gfx_puts("USART OFF");
		}
		lcd_show_frame();
		
		
	}
}
