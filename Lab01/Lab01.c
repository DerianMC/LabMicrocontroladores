#include <pic14/pic12f675.h>
 
//To compile:
//sdcc -mpic14 -p16f675 blink.c
 
//To program the chip using picp:
//Assuming /dev/ttyUSB0 is the serial port.
 
//Erase the chip:
//picp /dev/ttyUSB0 16f887 -ef
 
//Write the program:
//picp /dev/ttyUSB0 16f887 -wp blink.hex
 
//Write the configuration words (optional):
//picp /dev/ttyUSB0 16f887 -wc 0x2ff4 0x3fff
 
//Doing it all at once: erasing, programming, and reading back config words:
//picp /dev/ttyUSB0 16f887 -ef -wp blink.hex -rc
 
//To program the chip using pk2cmd:
//pk2cmd -M -PPIC16f887 -Fblink.hex

typedef unsigned int word ;
word __at 0x2007 __CONFIG = (_BODEN_OFF ) ;
 
void delay (unsigned inttiempo);
void dis (unsigned num);
 
void main(void)
{
	
  TRISIO = 0b00000000; //Poner todos los pines como salidas
	GPIO = 0x00; //Poner pines en bajo
	
 
  	unsigned int time = 1;
	unsigned int num = 1;
 
    //Loop forever
    while ( 1 )
    {
		num = 1;
		dis(num);
		
		delay(time);
		num = 2;
		dis(20);
		
		delay(time);
			
    }
 
}

void delay(unsigned int tiempo)
{
	unsigned int i;
	unsigned int j;

	for(i=0;i<tiempo;i++)
	  for(j=0;j<1275;j++);
}

void dis (unsigned int num)
{


	if (num == 1){
		GPIO = 0b00000100;

	}
	else {
		GPIO = 0b00100011;
	}

	

 // for (int i = 2; i < 9; i++)
  //{
  //  digitalWrite(i, bitRead(digit[num], i - 2));
  //}
}
