#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned char seconds=0 , mins=0 , hours=0;

void Timer1_setting(void)    // Timer1 settings
{
	TCCR1A= (1<<FOC1A)|(1<<FOC1B);
	TCCR1B=(1<<CS12)|(1<<CS10)|(1<<WGM12);
	TIMSK|=(1<<OCIE1A);
	TCNT1=0;     // initializing timer
	OCR1A=976;  // Output compare register value will be 976 not 1000 for more accurate

}

// Calculating Seconds , Minutes , Hours

ISR(TIMER1_COMPA_vect)
{

	seconds=seconds%60; // To make sure that seconds are in range of 60
	if (seconds==59)
	{
		mins++;
	}

	if(seconds<59)
	{
		seconds++;
	}
	else
	{
		seconds=0;
	}


	if(seconds==59&& mins==59)
	{
		hours++;
		mins=0;
	}

}



// RESET 7 Segments

void EXT_INT0(void)
{
	MCUCR|=(1<<ISC01);  // Initializing falling edge
	MCUCR &= ~(1<<ISC00);
	GICR|=(1<< INT0);
	DDRD &= ~(1<<PD2);   // Push button for reset
	PORTD |= (1<<PD2);   // Internal pull-up resistor

}

ISR(INT0_vect)
{

	seconds=0,mins=0,hours=0; // Reset Timer
}


// Stop Timer

void EXT_INT1(void)
{
	MCUCR|=(1<<ISC11)|(1<<ISC10);  /// External Interrupt INT1 with raising edge
	GICR|=(1<< INT1);
	DDRD &= ~(1<<PD3);   //a push button with the external pull-down resistor

}


ISR(INT1_vect)
{
	TIMSK&= ~(1<<OCIE1A);  // Disable Clock

}

// Resume Timer

void EXT_INT2(void)
{
	MCUCSR&= ~(1<<ISC2); // External Interrupt INT2 with falling edge
	GICR|=(1<<INT2);
	DDRB &= ~(1<<PB2);   // a push button with the internal pull-up resistor
	PORTB |= (1<<PB2);

}



ISR(INT2_vect)
{
	TIMSK|=(1<<OCIE1A); // Enable Clock

}





int main(void)
{
	SREG|=(1<<7);
	DDRC|=0x0f;
	DDRA |=0X3f;
	PORTA |=0x00;
	PORTC =0xC0;

	Timer1_setting();
	EXT_INT0();
	EXT_INT1();
	EXT_INT2();

	while(1)
	{


		// displaying seconds
		PORTA &=0x00;
		PORTA |=0x20;
		PORTC=seconds%10;
		_delay_ms(2);

		PORTA &=0x00;
		PORTA|=0x10;
		PORTC=seconds/10;
		_delay_ms(2);

		// displaying minutes
		PORTA &=0x00;
		PORTA |=0x08;
		PORTC=mins%10;
		_delay_ms(2);

		PORTA &=0x00;
		PORTA|=0x04;
		PORTC=mins/10;
		_delay_ms(2);


		// displaying hours
		PORTA &=0x00;
		PORTA |=0x02;
		PORTC =hours%10;
		_delay_ms(2);

		PORTA &=0x00;
		PORTA |=0x01;
		PORTC=hours/10;
		_delay_ms(2);
	}
}
