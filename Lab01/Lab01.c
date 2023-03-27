
#include <pic14/pic12f683.h>



typedef unsigned int word ;
word __at 0x2007 __CONFIG = (_CP_OFF & _PWRTE_ON & _WDT_OFF & _INTRC_OSC_NOCLKOUT & _FCMEN_OFF & _IESO_OFF & _MCLRE_OFF);


void delay(unsigned int tiempo);	
void dis(unsigned char num);
unsigned char var1 = 0;
unsigned char var2 = 0;
unsigned char value = 0;

 
char digit1[10] = {0b000000, 0b000001, 0b000010,
					0b000011, 0b000100, 0b000101,
					0b000110, 0b000111, 0b010000,
					0b010001};

char digit2[10] = {0b100000, 0b100001, 0b100010,
					0b100011, 0b100100, 0b100101,
					0b100110, 0b100111, 0b110000,
					0b110001}; 
					





void main() {
    // Set GP1 and GP2 as outputs
    TRISIO = 0b00001000; 
    GPIO = 0b000000;   
  	unsigned int time = 1;
	unsigned char num = 1;
	unsigned char seed = 0;
	unsigned char var1 = 0;
	unsigned char var2 = 0;
	


    while (1) {

		num++;
		unsigned char input_value = GP3;
		
		/*while (value == 0)
		{
		var1 = (num - (num / 100)*100)/10;
		var2 = (num - (num / 100)*100-((num - (num / 100)*100)/10)*10);	
		}
		
		GPIO = digit1[var1];
		delay(time);

		GPIO = digit2[var2];
		delay(time);	*/
		if (input_value == 1) {
			var1 = (num - (num / 100)*100)/10;
			var2 = (num - (num / 100)*100-((num - (num / 100)*100)/10)*10);	
			GPIO = digit1[0];
			delay(time);
			GPIO = digit2[0];
			delay(time);
		} 
		
		else {
			GPIO = digit1[var1];
			delay(time);
			GPIO = digit2[var2];
			delay(time);

		}
	






    }
}



void delay(unsigned int tiempo)
{
	unsigned int i;
	unsigned int j;

	for(i=0;i<tiempo;i++)
	  for(j=0;j<1275;j++);
}

