#include <pic14/pic12f683.h>



typedef unsigned int word ;
word __at 0x2007 __CONFIG = (_CP_OFF & _PWRTE_ON & _WDT_OFF & _INTRC_OSC_NOCLKOUT & _FCMEN_OFF & _IESO_OFF & _MCLRE_OFF);

//Se definen dos funciones, delay y blink 
void delay(unsigned int tiempo);	
void blink(void);




 //Se utiliza este array para tener los números a utilizar en GPIO
 // Se utilizan números de manera que los bit del cero al uno se asignen en los puertos GPIO0, GPIO1, GPIO2 y GPIO4
 //En GPIO3 se estableció una entrada para cuando se desea recibir un nuevo número
char digit[10] = {0b000000, 0b000001, 0b000010,
					0b000011, 0b000100, 0b000101,
					0b000110, 0b000111, 0b010000,
					0b010001};


					





void main() {
	//Asigna todos los pines como entradas, excepto GPIO3
    TRISIO = 0b00001000; 
	//Establece todos pines en bajo
    GPIO = 0b000000;   
	//Se generan variabler a utilizar a lo largo del código
	//time: contador para delay
	//num: número que aumentara cada vez que se repita while(1), funciona como número pseudo aleatorio
	//var1 y var2: se utilizan como los digitos del número aleatorio a mostrar
	//counter1 y counter2: utilizados para llevar un conteo de la cantidad de digitos que se han mostrado
	//mem[16]: guardará un registro de los números que ya se han mostrado 
  	unsigned int time = 1;
	unsigned char num = 1;
	unsigned char var1 = 0;
	unsigned char var2 = 0;
	unsigned char counter1 = 0;
	unsigned char counter2 = 0;
	unsigned char mem[16];
	


    while (1) {

		//Aumenta num en 1
		//Asigna input_value como el valor de la entrada GPIO3
		num++;
		unsigned char input_value = GP3;
		

		//Si faltan números por mostrar se entra a este bucle
		if (counter1 < 16){
			//Asigna a counter2 en valor de counter1
			//Esto para mantener el valor del contador cuando se repita el bucle while
			counter2 = counter1;
			while (input_value == 1)	//Se entra en bucle si se presiona botón
			{
				
				//Mientras se presiona el botón la pantalla se apaga asignando a GPIO 0x17 y 0x37
				GPIO = 0x17;
				delay(time);
				GPIO = 0x37;
				delay(time);
				input_value = GP3;		//Actualiza el valor de input_value para terminar ciclo cuando se deja de presionar botón
				if (counter1==counter2){		//Si counter1 y counter2 son iguales entonces se entra a if
					unsigned char i;
					for(i=0;i<counter1;i++){		//Recorre el arreglo de mem para revisar si hay valor repetido
						if (mem[i] == num){			
							num = num*i;			//Asigna a num un número pseudo aleatorio si ya se encuentra en memoria
						}
						else{
							mem[counter1] = num;	//Si número no está repetido lo asigna a memoria 
						}
					}
					counter1++;					//Aumenta contador en 1
				}

				//Asigna a var1 y var2 los valores de las unidades y decenas del num, número pseudo aleatorio
				var1 = (num - (num / 100)*100)/10;
				var2 = (num - (num / 100)*100-((num - (num / 100)*100)/10)*10);
			}
			
			
			
			GPIO = digit[var1] | 0b000000;			//Asigna a GPIO uno de los números aleatorios
			delay(time);
			GPIO = digit[var2] | 0b100000;			//Asigna a GPIO uno de los números aleatorios. Con la mascara y la OR se logra que el GPIO5 se encienda y con esto se cambie el display mostrado
			delay(time);							//Mantiene el programa con las salidas durante cierto tiempo
			
			
		}
		else {			//Si el contador es de 16 entonces se pasa a este código 

			//Nuevamente asigna los valores de unidades y decenas a var1 y var2
			//Luego muestra esos valores en el display 
			

			var1 = (mem[15] - (mem[15] / 100)*100)/10;
			var2 = (mem[15] - (mem[15] / 100)*100-((mem[15] - (mem[15] / 100)*100)/10)*10);
			GPIO = digit[var1] | 0b000000;
			delay(time);
			GPIO = digit[var2] | 0b100000;
			delay(time);
			counter2 = counter1;
			while (input_value == 1)		//Si se presiona de nuevo el botón luego de haber mostrado 16 números, entonces se reinicia el bingo
			{
				input_value = GP3;
				if (counter1==counter2){
					blink();				//Llama a función blink que se encarga de parpadeo de los display
					var1 = 0;				//Se reinician los valores a mostrar a 0 para mostrar que se está en espera de mostrar nuevos números
					var2 = 0;
					counter1 = 0;			//Se reinician contadores para mostrar nuevamente 16 números
					counter2 = 0;
				}
			}
			
		}
		
		

    }
}


//Función encargada de agregar retardos 
void delay(unsigned int tiempo)
{
	unsigned int i;
	unsigned int j;

	for(i=0;i<tiempo;i++)
	  for(j=0;j<1275;j++);
}



//Función encargada de hacer parpadear display 
//Logra que pantalla se apague usando un compuerta AND que recibe GPIO4 y GPIO2
//Se pudo haber utilizado un ciclo for para lograrlo, pero haciendolo de esa manera se consumió demasiada memoria
//Realizandolo de la manera mostrada no se excedió el límite de memoria 
void blink(void){
	GPIO = 0x17;
    delay(80);

	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);




    GPIO = 0x17;
    delay(80);
	
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);



    GPIO = 0x17;
    delay(80);


	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);
	GPIO = 0b010001;
    delay(5);
	GPIO = 0b110001;
    delay(5);




    GPIO = 0x17;
    delay(80);
	
	
}

