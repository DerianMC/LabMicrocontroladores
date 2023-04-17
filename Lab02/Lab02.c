#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//Definición de función encarda de mostrar el número de 2 dígitos.
void display(unsigned char num);

//Se crea un arreglo para cada etapa del proceso.
//En estos arreglos se guardan los tiempos correspondientes a cada etapa.
unsigned char suminis_agua[3] = {1, 2, 3};
unsigned char lavar[3] = {3, 7, 10};
unsigned char enjuagar[3] = {2, 4, 5};
unsigned char centrifugar[3] = {3, 6, 9};

//count1: contador para el display.
//count2: contador para la cuenta regresiva.
//conm: variable utilizada para la conmutación del display.
//COUNT_DOWN: cuenta regresiva, en esta variable se asignaran los valores para cada etapa.
//nxt_count2: guarda el valor del contador para que este se detenga en un valor determinado cuando se da pausa.
//MODO: guarda el modo de operación especificado.
//start: bandera de inicio del proceso. Se pone en 1 cuando se da en el botón de INICIO/PAUSA.
int count1 = 0x00;
int count2 = 0x00;
int conm = 0x00;
int COUNT_DOWN = 0x62;int nxt_count2 = 0x00;
int MODO = 3;
int start = 0;

//Enumeración de los estados.
typedef enum{
  STAND_BY,
  LLENADO,
  LAVADO,
  ENJUAGUE,
  CENTRIFUGADO,
  PAUSA
} STATES;

//Se crean variables tipo STATES para el movimiento entre estados.
//next_state: especifica el siguiente estado.
//state: contiene estado actual.
//prev_state: contiene el estado anterior. Esto para cuando se realiza una pausa.
STATES next_state;
STATES state;
STATES prev_state;


void init_interrupts() {
  sei();  //Habilita interrupciones globales.
  PCMSK |= (1 << PCINT6) | (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10);    //Habilita interrupciones en los pines PCINT 6,8,9,10.
  GIMSK |= (1 << PCIE0);      //Habilita interrupciones en los pines de PCMSK.

  
  TCCR0A=0x00;   //Establecer timer en modo normal.
  TCCR0B=0x00;    //Establecer timer en modo normal.
  TCCR0B |= 0b00000011; //Hace preescalado a 64.
  TCNT0=0;     //Resetea timer.
  OCR0A = 125;    //Establece el valor a comparar para la interrupción de timer.
  TIMSK |= (1 << OCIE0A);   //Habilita la interrupción por comparación.
}
//establecido cambio cada milisegundo.

//El siguiente código es el encargado de lidiar con las interrupciones de reloj.
ISR(TIMER0_COMPA_vect) {
  //Primer if realiza comparación de counter1.
  //Cuando se alcanzan 8 ms se invierte conm y count1 vuelve a cero.
  if(count1 == 8){
    conm = !conm;
    count1 = 0x00;
  }
  else count1++;

  //Segundo if realiza comparación de counter2.
  //Cuando counter2 alcanza 1000 ms o 1 s decremente COUNT_DOWN.
  //Esto en los estados distintos a PAUSA y a STAND_BY.
  if(count2 == 1000){

    if((state == PAUSA) | (state == STAND_BY)){
      ;
    }
    else{
      COUNT_DOWN--;
    }
    count2 = 0x00;
  }
  else count2++;
}

//En el siguiente ISR se lidia con las interrupciones generadas por lo botones.
ISR(PCINT0_vect) {
  //El primer if se encarga del botón INICIO/PAUSA
  if (PINB & (1 << PB6)) {
    //Si se está en STAND_BY se establece start en 1 para iniciar.
    if(state == STAND_BY){
      if(MODO != 3) start = 1;    //Se verifica que ya se haya ingresado un modo de operación.
      
    }
    //Si el estado actual es PAUSA entonces el programa vuelve a su funcionamiento.
    //Continua con el estado previo, esto cambiando next_state por prev_state.
    else if(state == PAUSA){
      next_state = prev_state;
    }
    //Si se encuentra en un estado distindo a PAUSA y STAND_BY entonces se entra a estado de PAUSA.
    else{
      
        prev_state = state;   //Guarda estado anterior en prev_state.
        nxt_count2 = count2;  //Guarda en nxt_count2 el valor actual del contador. 
        next_state = PAUSA;   //Se establece como siguiente estado PAUSA.
      
    }
    }
  //If encargado de botón para carga baja. Establece la variable MODO en 0.
  else if (PINA & (1 << PA0)) {
      MODO = 0;
    }
  //If encargado de botón para carga media. Establece la variable MODO en 1.
  else if (PINA & (1 << PA1)) {
      MODO = 1;
    }
  //If encargado de botón para carga alta. Establece la variable MODO en 2.
  else if (PINA & (1 << PA2)) {
      MODO = 2;
    }
}




int main(void)
{
  init_interrupts();  //Se configuran las interrupciones.
  DDRA = 0x00;        //se establecen los pines A como entradas.
  DDRB = 0x3F;        //Se establecen los pines B como salidas, excepto el 6, que se utiza como entrada.
  DDRD = 0xFF;        //Se establecen los pines D como salidas.
  

  state = STAND_BY;   //Primer estado es STAND_BY.

  
  while (1) {

    //En este if se revisa si el pin PB6 está en alto.
    //Si está en alto no se cambia de estado.
    //Cuando ya está en bajo se realiza el cambio de estado.
    //Se hizo de esta manera para que se pase de estado una vez se haya soltado el pulsador.
    //De esta manera no se entra en otro estado por error.
    if(PINB & (1 << PB6)){
      ;
    }
    else{
      state = next_state;
    }
    
    //Se inicia con máquina de estados.
    //Se implementa con un switch case.
    switch (state)
    {
    
    //Primer estado.
    //En este estado se mantiene el sistema en espera de recibir el modo de operación y señal de inicio.
    //Primeramente dependiendo del modo de operación se enciende un LED indicativo.
    //En el segundo if se verifica que se presiona el botón de start.
    //Previamente se configura para que start solo se ponga en alto si ya se estableció un modo de operacuón.
    case STAND_BY:
        display(99);
        if(MODO == 0){
          PORTD = 0x01;
        }
        else if(MODO == 1){
          PORTD = 0x02;
        }
        else if(MODO == 2){
          PORTD = 0x04;
        }
        if(start == 1){
            COUNT_DOWN = suminis_agua[MODO];
            count2 = 0;
            next_state = LLENADO;
          }
      break;

    //Segundo estado. Corresponde a etapa de Suministro de agua.
    //Primero llama la función display y muestra la cuenta regresiva.
    //Luego se establece el pin PD3 en alto, encendiendo LED indicativo de etapa.
    //En el if se verifica si la cuenta regresiva es menor a 0.
    //Si es menor a cero se asigna el nuevo valor de COUNT_DOWN. Para esto usa arrays definidos al principio, para cada etapa.
    //Se reinicia count2 para que se cuente un segundo completo.
    //Se establece next_state como LAVADO.
    case LLENADO:
        display(COUNT_DOWN);
        PORTD |= (1 << 3);
        if(COUNT_DOWN < 0){
          COUNT_DOWN = lavar[MODO];
          count2 = 0;
          next_state = LAVADO;
        }
      break;

    //Tercer estado. Corresponde etapa de Lavar.
    //Primero llama la función display y muestra la cuenta regresiva.
    //Se establece pin PD3 en bajo, apagando LED indicativo de etapa anterior.
    //Luego se establece el pin PD4 en alto, encendiendo LED indicativo de etapa.
    //En el if se verifica si la cuenta regresiva es menor a 0.
    //Si es menor a cero se asigna el nuevo valor de COUNT_DOWN. Para esto usa arrays definidos al principio, para cada etapa.
    //Se reinicia count2 para que se cuente un segundo completo.
    //Se establece next_state como ENJUAGUE.
    case LAVADO:
      display(COUNT_DOWN);
      PORTD &= ~(1 << 3);
      PORTD |= (1 << 4);
      if(COUNT_DOWN < 0){
          COUNT_DOWN = enjuagar[MODO];
          count2 = 0;
          next_state = ENJUAGUE;
        }
      
      break;

    //Cuarto estado. Corresponde a etapa de Enjuagar.
    //Primero llama la función display y muestra la cuenta regresiva.
    //Se establece pin PD4 en bajo, apagando LED indicativo de etapa anterior.
    //Luego se establece el pin PD5 en alto, encendiendo LED indicativo de etapa.
    //En el if se verifica si la cuenta regresiva es menor a 0.
    //Si es menor a cero se asigna el nuevo valor de COUNT_DOWN. Para esto usa arrays definidos al principio, para cada etapa.
    //Se reinicia count2 para que se cuente un segundo completo.
    //Se establece next_state como CENTRIFUGADO.
    case ENJUAGUE:
      display(COUNT_DOWN);
      PORTD &= ~(1 << 4);
      PORTD |= (1 << 5);
      if(COUNT_DOWN < 0){
          COUNT_DOWN = centrifugar[MODO];
          count2 = 0;
          next_state = CENTRIFUGADO;
        }
      break;

    //Quinto estado. Corresponde a etapa de Centrifugar.
    //Primero llama la función display y muestra la cuenta regresiva.
    //Se establece pin PD5 en bajo, apagando LED indicativo de etapa anterior.
    //Luego se establece el pin PD6 en alto, encendiendo LED indicativo de etapa.
    //En el if se verifica si la cuenta regresiva es menor a 0.
    //Si es menor a cero se asigna el nuevo valor de COUNT_DOWN. Para esto usa arrays definidos al principio, para cada etapa.
    //Se reinicia count2 para que se cuente un segundo completo.
    //Se establece next_state como ENJUAGUE.
    case CENTRIFUGADO:
      display(COUNT_DOWN);
      PORTD &= ~(1 << 5);
      PORTD |= (1 << 6);
      if(COUNT_DOWN < 0){
          count2 = 0;
          next_state = STAND_BY;
          MODO = 3;
          start = 0;
          PORTD = 0x00;
        }
      break;

    //Estado de pausa.
    //En este estado se entra cuando se presiona el botón de INICIO/PAUSA mientras se está en alguna de las etapas.
    //Primeramente muestra el valor de COUNT_DOWN.
    //Por la manera en que se configuró, COUNT_DOWN no cambia en este estado.
    //De esta manera el valor en el display no cambia.
    //Luego se establece que count2 es igual al valor de count2 que se tenía cuando se presionó la pausa.
    //El valor de count2 cuando se presionó pausa se guardó en nxt_count2.
    case PAUSA:
      display(COUNT_DOWN);
      count2 = nxt_count2;
      break;

    //Por default se tiene el estado STAND_BY.
    //No se debería llegar al estado default.
    //Se establece para evitar indefiniciones en caso de errores.
    default:
      next_state = STAND_BY;
      break;
    }
    

  }
}


//Función encargada de mostrar números en el display.
//Primero se crean dos variables.
//Luego se entra en un if. 
//En el if si se está en el estado STAND_BY se asignan valores numéricos para mostrarlos en display.
//En cualquier otro estado se realiza la operación para obtener el valor de los dos últimos digitos.
//En el segundo if se realiza una acción dependiendo de la variable conm.
//Está variable cambia su valor valor dependiendo del timer y de count1.
//Cada vez que count1 alcanza cierto valor conm cambia.
//En cada cambio muestra el contenido en cada uno de los displays.
void display(unsigned char num){
  unsigned char var1;
  unsigned char var2;

  var1 = (num - (num / 100)*100)/10;
	var2 = (num - (num / 100)*100-((num - (num / 100)*100)/10)*10);

  if(state == STAND_BY){
    var1 = 0x0A;
    var2 = 0x0E;
  }
  else{
    var1 = (num - (num / 100)*100)/10;
	  var2 = (num - (num / 100)*100-((num - (num / 100)*100)/10)*10);
  }
  if (conm == 0x00){
    PORTB = (var1 | 0x10);
  }
  else{
    PORTB = (var2 | 0x20);
  }
}