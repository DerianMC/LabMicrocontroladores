#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



void display(unsigned char num);
char digit[10] = {0b000000, 0b000001, 0b000010,
					0b000011, 0b000100, 0b000101,
					0b000110, 0b000111, 0b001000,
					0b001001};

int count1 = 0x00;
int com = 0x00;

void init_interrupts() {
  sei();
  PCMSK |= (1 << PCINT6) | (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10);
  GIMSK |= (1 << PCIE0);

  TCCR0A=0x00;   //Normal mode
  TCCR0B=0x00;
  //TCCR0B |= (1 << CS01) | (1 << CS00);   //prescaling with 1024
  TCCR0B |= 0b00000011; //prescaling with 64
  TCNT0=0;
  OCR0A = 125;
  TIMSK |= (1 << OCIE0A);

  //TIMSK|=(1<<TOIE0);

  /*
  TCCR0A |= (1 << WGM01);
  TCCR0B |= (1 << CS01) | (1 << CS00);
  OCR0A = 124;
  TIMSK |= (1 << OCIE0A);*/
}


/*void timer_setup(){
  TCCR0A=0x00;   //Normal mode
  TCCR0B=0x00;
  TCCR0B |= (1<<CS00)|(1<<CS02);   //prescaling with 1024
  TCNT0=0;
  TIMSK|=(1<<TOIE0); //enabling timer0 interrupt
  PORTB|=(1<<PB1);
}*/

ISR(TIMER0_COMPA_vect) {
  if(count1 == 8){
    com = !com;
    count1 = 0x00;
    
  }
  else count1++;
}


ISR(PCINT0_vect) {

  if (PINB & (1 << PB6)) {
      PORTB = digit[9] | 0x10;
      _delay_ms(5000);
    }
  else if (PINA & (1 << PA0)) {
      PORTB = digit[9] | 0x10;
      _delay_ms(5000);
    }
  else if (PINA & (1 << PA1)) {
      PORTB = digit[9] | 0x10;
      _delay_ms(5000);
    }
  else if (PINA & (1 << PA2)) {
      PORTB = digit[9] | 0x10;
      _delay_ms(5000);
    }
}




int main(void)
{
  init_interrupts();

  DDRA = 0x00;
  DDRB = 0x3F;
  DDRD = 0xFF;

  unsigned char num = 56;
  
  


  while (1) {
    display(num);
    

  }
}


void display(unsigned char num){
  unsigned char var1;
  unsigned char var2;

  var1 = (num - (num / 100)*100)/10;
	var2 = (num - (num / 100)*100-((num - (num / 100)*100)/10)*10);


  if (com == 0x00){
    PORTB = (var1 | 0x10);
  }
  else{
    PORTB = (var2 | 0x20);
  }
  
  
  /*PORTB = (var1 | 0x10);
  _delay_ms(50);
  PORTB = (var2 | 0x20);
  _delay_ms(50);*/

}