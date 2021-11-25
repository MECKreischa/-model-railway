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

unsigned char status = 0;
volatile unsigned char count;

void timer1 (void);
void master_init (void);
void master_transmit (unsigned char data);

SIGNAL (SIG_SPI) {
	return;
}

SIGNAL (SIG_OVERFLOW1) {						//Senderoutine
	if (count == 1) {
		master_transmit ('1');
		count--;
		return;
	}
	if (count == 0) {
		master_transmit ('0');
		count++;
	}
}

void timer1 (void) {
	TIMSK |= (1<<TOIE1);           				//Timer Overflow Interrupt enable
	TCNT1 = 0;                					//R�cksetzen des Timers
	TCCR1B = (1<<CS10) | (1<<CS11);			//8MHz/65536/64 = 1,91Hz --> 0,5s
}

void master_init (void) {
	DDRB = (1<<PB0) | (1<<PB3) | (1<<PB5);		// setze SCK,MOSI,PB0 (SS) als Ausgang
	DDRB &= ~(1<<PB4);							// setze MISO als Eingang
	PORTB = (1<<PB5) | (1<<PB0);				// SCK und PB0 high (ist mit SS am Slave verbunden)
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);	//Aktivierung des SPI, Master, Taktrate fck/16
	status = SPSR;								//Status l�schen
}

void master_transmit (unsigned char data) {
	PORTB &= ~(1<<PB0);						//SS am Slave Low --> Beginn der �bertragung
	SPDR = data;								//Schreiben der Daten
	while (!(SPSR & (1<<SPIF)));
	PORTB |= (1<<PB0);							//SS High --> Ende der �bertragung
}

int main (void) {

	master_init ();
	timer1 ();
	sei ();

	for (;;);
	return 0;
}
