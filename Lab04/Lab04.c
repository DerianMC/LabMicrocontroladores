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
#include "clock.h"
#include "console.h"
#include "sdram.h"
#include "lcd-spi.h"
#include "gfx.h"



#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include "clock.h"
#include "console.h"

#define L3GD20_SENSITIVITY_250DPS  (0.00875F)  

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
}

//-----------------------------------------------

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
	bool USART = false; 

	//-----------------------------------------------------------------------
	while (1) {

		if (gpio_get(GPIOA, GPIO0)) {
			while (gpio_get(GPIOA, GPIO0))
			{
				__asm__("nop");
			}
			USART = !USART;
		}
		if(USART == true){
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
			console_puts(axes[i]);
			tmp = vecs[i] - baseline[i];
			pad = print_decimal(tmp);
			pad = 15 - pad;
			while (pad--) {
				console_puts(" ");
			}
		}
		console_putc('\r');
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


		gfx_setTextColor(LCD_BLUE, LCD_BLACK);
		gfx_setTextSize(2);

		gfx_fillScreen(LCD_BLACK);
		gfx_setCursor(10, 36);
		gfx_puts("Sismografo UCR");

		gfx_setTextColor(LCD_YELLOW, LCD_BLACK);
		gfx_setTextSize(1);

		gfx_setCursor(100, 106);
		gfx_puts("x=");
		gfx_setCursor(120, 106);
		gfx_puts(a);
		gfx_setCursor(100, 126);
		gfx_puts("y=");
		gfx_setCursor(120, 126);
		gfx_puts(b);
		gfx_setCursor(100, 146);
		gfx_puts("z=");
		gfx_setCursor(120, 146);
		gfx_puts(c);
		if(USART == true){
			gfx_setTextSize(2);
			gfx_setCursor(100, 300);
			gfx_puts("USART ON");
		}
		else{
			gfx_setTextSize(2);
			gfx_setCursor(80, 300);
			gfx_puts("USART OFF");
		}
		lcd_show_frame();
		
	}
}
