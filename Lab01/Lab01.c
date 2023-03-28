#include <pic14/pic12f683.h>



typedef unsigned int word ;
word __at 0x2007 __CONFIG = (_CP_OFF & _PWRTE_ON & _WDT_OFF & _INTRC_OSC_NOCLKOUT & _FCMEN_OFF & _IESO_OFF & _MCLRE_OFF);

//Se definen 
void delay(unsigned int tiempo);	
void blink(void);




 
char digit[10] = {0b000000, 0b000001, 0b000010,
					0b000011, 0b000100, 0b000101,
					0b000110, 0b000111, 0b010000,
					0b010001};


					





void main() {
    // Set GP1 and GP2 as outputs
    TRISIO = 0b00001000; 
    GPIO = 0b000000;   
  	unsigned int time = 1;
	unsigned char num = 1;
	unsigned char var1 = 0;
	unsigned char var2 = 0;
	unsigned char counter1 = 0;
	unsigned char counter2 = 0;
	unsigned char mem[16];
	


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

		if (counter1 < 16){
			counter2 = counter1;
			while (input_value == 1)
			{
				
					
				GPIO = 0x17;
				delay(time);
				GPIO = 0x37;
				delay(time);
				input_value = GP3;
				if (counter1==counter2){
					unsigned char i;
					for(i=0;i<counter1;i++){
						if (mem[i] == num){
							mem[counter1] = num*counter1;
						}
						else{
							mem[i] = num;
						}
					}
					counter1++;
				}
				var1 = (num - (num / 100)*100)/10;
				var2 = (num - (num / 100)*100-((num - (num / 100)*100)/10)*10);
			}
			
			
	
			GPIO = digit[var1] | 0b000000;
			delay(time);
			GPIO = digit[var2] | 0b100000;
			delay(time);
			
			
		}
		else{
			blink();
			var1 = 0;
			var2 = 0;
			GPIO =  0b000000;
			GPIO =  0b100000;
			counter1 = 0;
		}
		/*else{
			unsigned int i;
			unsigned int j;

			for(i=0;i<100000000;i++)
				for(j=0;j<1275;j++){
					GPIO = digit1[9];
					delay(time);
					GPIO = digit2[9];
					delay(time);
				}

			GPIO = 0x17;
			delay(100000000);
			
			GPIO = 0x17;
			delay(100000000);


			
		}*/


		/*
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

		}*/



		/*while (input_value == 1)
		{
			
			var1 = (num - (num / 100)*100)/10;
			var2 = (num - (num / 100)*100-((num - (num / 100)*100)/10)*10);	
			GPIO = 0x17;
			delay(time);
			GPIO = 0x37;
			delay(time);
			input_value = GP3;
		}
		
	
		GPIO = digit1[var1];
		delay(time);
		GPIO = digit2[var2];
		delay(time);*/





    }
}



void delay(unsigned int tiempo)
{
	unsigned int i;
	unsigned int j;

	for(i=0;i<tiempo;i++)
	  for(j=0;j<1275;j++);
}

void blink(void){
	unsigned int i;

	
	
	



    // Turn off the display
    GPIO = 0x17;
    delay(500);
}

