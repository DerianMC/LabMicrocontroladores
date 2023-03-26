#include <pic14/pic12f675.h>
 

typedef unsigned int word ;
word __at 0x2007 __CONFIG = (_BODEN_OFF ) ;
 
void delay (unsigned inttiempo);
void dis (unsigned num);


 
char digit1[10] = {0b000000, 0b000001, 0b000010,
					0b000011, 0b000100, 0b000101,
					0b000110, 0b000111, 0b010000,
					0b010001};

char digit2[10] = {0b100000, 0b100001, 0b100010,
					0b100011, 0b100100, 0b100101,
					0b100110, 0b100111, 0b110000,
					0b110001}; 


void main(void)
{
	
  	TRISIO = 0b00000000; //Poner todos los pines como salidas
	TRISIO3 = 0b00000001;
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
		dis(num);
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
		GPIO = digit1[1];
	}
	else {
		GPIO = digit2[3];
	}
}

void calculoNumero(unsigned int clock, unsigned int* num1, unsigned int* num2){
	static unsigned int value = 0;
    if (clock != 0) {
        value = clock;
    }
    value = (value * 1664525UL + 1013904223UL) % 4294967296UL;
    return;
}
