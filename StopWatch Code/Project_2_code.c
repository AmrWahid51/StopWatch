#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int sec1 = 0;
int sec2 = 0;
int min1 = 0;
int min2 = 0;
int hour1 = 0;
int hour2 = 0;

void interrupt_0_init(void)
{

	// make PD2 Input pin and activate internal pull up resistance
	DDRD &= ~(1<<2);
	PORTD |= (1<<2);
	// adjust interrupt with falling edge
	MCUCR |= (1<<ISC01);
	MCUCR &= ~(1<<ISC00);
	// enable int 0 module flag
	GICR  |= (1<<INT0);

}

void interrupt_1_init(void)
{
	// adjust interrupt with rising edge
	MCUCR |= (1<<ISC10) | (1<<ISC11);
	// make PD3 input pin
	DDRD &= ~(1<<3);
	//enable int 1 module flag
	GICR  |= (1<<INT1);
}
void interrupt_2_init(void)
{
	// adjust interrupt with falling edge
	MCUCSR &= ~(1<<ISC2);
	// make PB2 Input pin and activate internal pull up resistance
	DDRB &= ~(1<<2);
	PORTB |= (1<<2);
	//enable int 2 module flag
	GICR  |=  (1<<INT2);
}


ISR(INT0_vect)
{
   // reset all counts and timer initial value to 0
	TCNT1 = 0;
	sec1 = 0;
	sec2 = 0;
	min1 = 0;
	min2 = 0;
	hour1 = 0;
	hour2 = 0;
	/* by setting these 2 bits to 1 timer will start after reset even if it was
	 stopped before reset button was pressed
	 */
	TCCR1B |= (1<<CS10) | (1<<CS12);

}


ISR(INT1_vect)
{

	TCCR1B &= ~(1<<CS10) & ~(1<<CS12); // by setting these 2 bits to 0 timer will stop
}

ISR(INT2_vect)
{

	TCCR1B |= (1<<CS10) | (1<<CS12); // by setting these 2 bits to 1 timer will continue
}

void display_7_segment(void)
{
   //loop on the 6 7Segments enable with a small delay to make them all display at the same time
	PORTA = (PORTA & 0XC0) |(1<<5);
	PORTC = (PORTC & 0XF0) | sec1;
	_delay_ms(3);
	PORTA = (PORTA & 0XC0) |(1<<4);
	PORTC = (PORTC & 0XF0) | sec2;
	_delay_ms(3);
	PORTA = (PORTA & 0XC0) |(1<<3);
	PORTC = (PORTC & 0XF0) | min1;
	_delay_ms(3);
	PORTA = (PORTA & 0XC0) |(1<<2);
	PORTC = (PORTC & 0XF0) | min2;
	_delay_ms(3);
	PORTA = (PORTA & 0XC0) |(1<<1);
	PORTC = (PORTC & 0XF0) | hour1;
	_delay_ms(3);
	PORTA = (PORTA & 0XC0) |(1<<0);
	PORTC = (PORTC & 0XF0) | hour2;
	_delay_ms(3);

}
void timer_1_init(void)
{


	TCCR1A = (1<< FOC1A); // get output on compare A
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); // choose prescalar 1024 and CTC mode

	TCNT1 = 0; // setting initial value to 0
	//1 cycle is 1.024  msecond so it will give interrupt every 977 cycle which is 1 second
	OCR1A = 977;

	// enable timer1 compare mode interrupt bit
	TIMSK = (1<< OCIE1A);



}

ISR(TIMER1_COMPA_vect)
{

	sec1++;

	if(sec1 == 10)
	{
		sec1 = 0;
		sec2++;
	}
	if(sec2 == 6)
	{
		sec2 = 0;
		min1++;
	}
	if(min1 == 10 )
	{
		min1 = 0;
		min2++;
	}
	if(min2 == 6)
	{
		min2 = 0;
		hour1++;

	}
	if(hour1 == 10)
	{
		hour1 = 0;
		hour2++;
	}



}


int main(void)
{

	//enable the I bit
	SREG |= (1<<7);
	// calling initialization functions
	interrupt_0_init();
	interrupt_1_init();
	interrupt_2_init();
	timer_1_init();

	//initializations

	DDRC |= 0X0F; 	// make 1st 4pins of portC output pins
	PORTC &= 0XF0; //display 0
	DDRA |= 0X3F; // make 1st 6pins of PortA output pins
	PORTA |= 0X3F; // enable the 6 7Segments


	while(1)
	{

		display_7_segment(); // call display function inside while(1) to loop on the 6 7Segments
	}


}
