#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



char digit[10] = {0b000000, 0b000001, 0b000010,
					0b000011, 0b000100, 0b000101,
					0b000110, 0b000111, 0b001000,
					0b001001};

void init_interrupts() {
  sei();
  PCMSK |= (1 << PCINT6);
  GIMSK |= (1 << PCIE0);
}

ISR(PCINT0_vect) {

  if (PINB & (1 << PB6)) {
      PORTB = digit[9] | 0x10;
      _delay_ms(5000);
    }

}

int main(void)
{
  init_interrupts();
  DDRA = 0xFF;
  DDRB = 0x3F;
  DDRD = 0x00;
  
  


  while (1) {
    PORTB = digit[5] | 0x10; //PORTB &= ~(1 << PB3); //Esto se puede hacer tambien asi
    _delay_ms(50); 
    PORTB = digit[6] | 0x20; //PORTB &= ~(1 << PB3); //Esto se puede hacer tambien asi
    _delay_ms(50); 

  }
}
