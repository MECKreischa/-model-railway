/**************************************************************
Es soll alle halbe Sekunde im Wechsel 0 bzw. 1 gesendet werden.
Am korrespondierenden Slave soll zur Indikation jeweils die 
LEDs an bzw. aus gehen
Verdrahtung:	MISO(Master) --> MISO(Slave)
				MOSI(Master) --> MOSI(Slave)
				SCK(Master)  --> SCK(Slave)
				PB0(Master)	 --> SS(Slave)
**************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

volatile unsigned char data;
unsigned char status;

SIGNAL (SIG_SPI) {
	data = SPDR;
	if (data == '1')	PORTD = 0x00;			//LEDs an
	if (data == '0')	PORTD = 0xff;			//LEDs aus
}

void slave_init (void) {
	DDRB |= (1<<PB4);							//MISO als Ausgang, der Rest als Eingang
	SPCR = (1<<SPE) | (1<<SPIE);				//Aktivierung des SPI + Interrupt
	status = SPSR;								//Status l�schen
}

int main (void) {

	DDRD = 0xff;
	slave_init ();
	sei ();
	
	for (;;);
	return 0;
}
