#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void display(unsigned char num);
char digit[10] = {0b000000, 0b000001, 0b000010,
					0b000011, 0b000100, 0b000101,
					0b000110, 0b000111, 0b001000,
					0b001001};

unsigned char suminis_agua[3] = {1, 2, 3};
unsigned char lavar[3] = {3, 7, 10};
unsigned char enjuagar[3] = {2, 4, 5};
unsigned char centrifugar[3] = {3, 6, 9};

int count1 = 0x00;
int count2 = 0x00;
int conm = 0x00;
int COUNT_DOWN = 0xF2;
int nxt_COUNT_DOWN = 0x00;
int MODO = 3;
int start = 0;

typedef enum{
  STAND_BY,
  LLENADO,
  LAVADO,
  ENJUAGUE,
  CENTRIFUGADO,
  PAUSA
} STATES;

STATES next_state;
STATES state;
STATES prev_state;


void init_interrupts() {
  sei();
  PCMSK |= (1 << PCINT6) | (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10);
  GIMSK |= (1 << PCIE0);

  TCCR0A=0x00;   //Modo normal
  TCCR0B=0x00;
  TCCR0B |= 0b00000011; //prescaling with 64
  TCNT0=0;
  OCR0A = 125;
  TIMSK |= (1 << OCIE0A);
}


ISR(TIMER0_COMPA_vect) {
  if(count1 == 8){
    conm = !conm;
    count1 = 0x00;
  }
  else count1++;

  if(count2 == 1000){

    if((state == PAUSA) | (state == STAND_BY)){
      COUNT_DOWN = COUNT_DOWN;
    }
    else{
      COUNT_DOWN--;
    }
    count2 = 0x00;
  }
  else count2++;
}


ISR(PCINT0_vect) {

  if (PINB & (1 << PB6)) {
    start = 1;
    }
  else if (PINA & (1 << PA0)) {
      MODO = 0;
    }
  else if (PINA & (1 << PA1)) {
      MODO = 1;
    }
  else if (PINA & (1 << PA2)) {
      MODO = 2;
    }
}




int main(void)
{
  init_interrupts();
  DDRA = 0x00;
  DDRB = 0x3F;
  DDRD = 0xFF;
  

  next_state = STAND_BY;

  
  while (1) {
    state = next_state;
    
    switch (state)
    {
    case STAND_BY:
        display(98);
        if(MODO == 0){
          PORTD = 0x01;
          
        }
        else if(MODO == 1){
          PORTD = 0x02;
        }
        else if(MODO == 2){
          PORTD = 0x04;
        }
        if((start == 1) & (MODO != 3)){
            COUNT_DOWN = suminis_agua[MODO];
            count2 = 0;
            next_state = LLENADO;
          }
      break;
    case LLENADO:
        display(COUNT_DOWN);
        if(COUNT_DOWN < 0){
          COUNT_DOWN = lavar[MODO];
          count2 = 0;
          next_state = LAVADO;
        }
      break;
    case LAVADO:
      display(COUNT_DOWN);
      if(COUNT_DOWN < 0){
          COUNT_DOWN = enjuagar[MODO];
          count2 = 0;
          next_state = ENJUAGUE;
        }
      
      break;
    case ENJUAGUE:
      display(COUNT_DOWN);
      if(COUNT_DOWN < 0){
          COUNT_DOWN = centrifugar[MODO];
          count2 = 0;
          next_state = CENTRIFUGADO;
        }
      break;
    case CENTRIFUGADO:
      display(COUNT_DOWN);
      if(COUNT_DOWN < 0){
          count2 = 0;
          next_state = STAND_BY;
          MODO = 3;
          start = 0;
          PORTD = 0x00;
        }
      break;
    case PAUSA:
      display(98);
      break;
    default:
      break;
    }
    

  }
}


void display(unsigned char num){
  unsigned char var1;
  unsigned char var2;

  var1 = (num - (num / 100)*100)/10;
	var2 = (num - (num / 100)*100-((num - (num / 100)*100)/10)*10);

  if (conm == 0x00){
    PORTB = (var1 | 0x10);
  }
  else{
    PORTB = (var2 | 0x20);
  }
}