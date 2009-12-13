#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include <avr/iom8.h>

#define ICP PINB0
#define BAUDRATE 9600

//Buffer to save data to send
volatile uint16_t buffer[32];
volatile int bufferRead = 0;
volatile int bufferWrite = 0;
volatile int newData = 0;
volatile int newline = 0;

//define ovrflow counter
uint8_t ov_counter;

//define times for start and end of signal
uint16_t start, end;

//define overall counts
uint16_t counts;

//Serial Initialization
void InitUART (unsigned int bittimer);
void TransmitByte (unsigned char data);
void serial16bitPrint(uint16_t data);
void serial8bitPrint(uint8_t data);
void serial32bitPrint(uint32_t data);

volatile unsigned int blink = 0;
volatile int blinkflag = 0;
volatile uint32_t output;

//overflow counter interrupts service routine
ISR(TIMER0_OVF_vect){
	blink = 1 + blink;
	if( blink > 60 ){
		blink = 0;
		blinkflag = 1;
	}
}

ISR(TIMER1_OVF_vect){
	if(newline == 1){
		buffer[bufferWrite] = 0x0000;
		bufferWrite++;
		newData++;
		if(bufferRead == 32){
			bufferRead = 0;
		}
		if(bufferWrite == 32){
			bufferWrite = 0;
		}
			newline = 0;
		}
}

//Timer1 capture interrupt service subroutine
ISR(TIMER1_CAPT_vect){
	cli();
	PORTB ^= _BV(4);
	buffer[bufferWrite] = TCNT1;
	bufferWrite++;
	newData++;
	newline = 1;

	if(bufferRead == 32){
		bufferRead = 0;
	}
	if(bufferWrite == 32){
		bufferWrite = 0;
	}

	//reset TCNT1
	TCNT1 = 0;

	sei();	//Enable Interupts
}

int main(void) {
  DDRB = 0xF8;
	//enable overflow and input capture interrupts
	TIMSK1 = 0x21;
	//TIMSK1=0x24;
	/*Noise canceller, without prescaler, rising edge*/
	TCCR1B=0x02;

	TCCR0B |= _BV(CS02) | _BV(CS00);

	/* Enable Timer Overflow Interrupts */
	TIMSK0 |= _BV(TOIE0);

	/* other set up */
	TCNT0 = 0;

	InitUART( F_CPU / BAUDRATE / 16 );	//Set up the baud rate
	
		TransmitByte('S');
		TransmitByte('t');
		TransmitByte('a');
		TransmitByte('r');
		TransmitByte('t');
		TransmitByte('P');
		serial16bitPrint(0xABCD);
		serial16bitPrint(0xEF00);
		serial32bitPrint(0xFFFFFFFF);
		serial32bitPrint(0x89ABCDEF);
		TransmitByte('\n');
	sei();	//Enable Interupts
	while(1==1) {
	if(blinkflag == 1)	{
		PORTB ^= _BV(5);
		blinkflag = 0;
	}
		/* Makes sure that the buffer never over flows.. this also
		 * makes it possible for old data to be over written with new.*/
	
		//serial8bitPrint(newData);
		if(newData != 0){
			//serial16bitPrint(buffer[bufferRead]);
			
			if(buffer[bufferRead] >= 0x1000)
			{
				//TransmitByte('1');
				output = (output << 1) + 1;
				//TransmitByte(' ');
				//serial32bitPrint(output);
				//TransmitByte('\n');

			} else if(buffer[bufferRead] == 0x0000) {
				//TransmitByte('\n');
				serial32bitPrint(output);
				TransmitByte('\n');
				output = 0;
				//Transmit the 32bit HEX
			} else {
				//TransmitByte('0');
				//TransmitByte(' ');
				output = (output << 1) & 0xFFFFFFFE;
				//serial32bitPrint(output);
				//TransmitByte('\n');
			}
				
			//TransmitByte(' ');
			bufferRead++;
			newData--;
			
		}
		
		/* loop forever timer does the job*/
	}

}

void InitUART (unsigned int bittimer)
{
 	/* Set the baud rate */
  UBRR0H = (unsigned char) (bittimer >> 8);
  UBRR0L = (unsigned char) bittimer;
  /* set the framing to 8N1 */
  UCSR0C = (3 << UCSZ00);
  /* Engage! */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  return;
}

void TransmitByte (unsigned char data)
{
  /* Wait for empty transmit buffer */
  while (!(UCSR0A & (1 << UDRE0)));

  /* Start transmittion */
  UDR0 = data;

}

void serial8bitPrint(uint8_t data){
	uint8_t thing1 = (( data & 0xF0 )>>4) + 48;
	uint8_t thing2 = ( data & 0x0F ) + 48;
	if(thing1 >= 58){
		thing1 += 7;
	}
	if(thing2 >= 58){
		thing2 += 7;
	}

  TransmitByte(thing1);
  TransmitByte(thing2);
}

void serial32bitPrint(uint32_t data){
	uint16_t top;
	uint16_t bottom;
	top = ((data & 0xFFFF0000) >> 16);
	bottom = (data & 0x0000FFFF);
	serial16bitPrint(top);
	serial16bitPrint(bottom);
}

void serial16bitPrint(uint16_t data){
	uint8_t top;
	uint8_t bottom;
	top = ((data & 0xFF00) >> 8);
	bottom = (data & 0x00FF);
	serial8bitPrint(top);
	serial8bitPrint(bottom);
}

