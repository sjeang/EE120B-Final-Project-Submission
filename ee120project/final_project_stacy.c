/*
 seang001_finalProject.c
 *
 * Stacy Eang, seang001@ucr.edu
 * Lab Section - B21
 * Assignment: Final Project: Simon
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 *
 * GccApplication1.c
 *
 * Created: 8/23/2017 1:23:24 PM
 *  Author: student
 */ 

/*
Simon
LCD welcome screen
Wait for button press to begin
Random sequence shown on led
Player enters sequence on button presses
Report score on LCD screen
Winning sequence is 9 button presses
Report win/lose on LCD screen
Bonus:  Add sounds with the LED/Button press
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "io.c"

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

unsigned char button;
unsigned char start;
unsigned int x;


void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR()
{
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}

unsigned char GetBit (unsigned char x, unsigned char k ) {
	return ((x & (0x01 << k)) != 0);
}


void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}
//------------------------------------------------------------


unsigned char score;
double arrayINPUT[9] = {};
unsigned char i;
unsigned char j;

unsigned char wait_input;

void assign_Inputs(unsigned char score_int){
	arrayINPUT[score_int] = rand() % 4;
	
}

enum WELCOME_STATE 
{
	init_welcome, 
	displayWait,
	displayGameStart,	
} WELCOME_STATE;

unsigned char button = 0x00;
unsigned char inc;
unsigned char z;

void welcome_screen()
{
	
	button = ~PINA & 0x10;
	
	switch(WELCOME_STATE) 
	{
		case init_welcome:
			LCD_init();
			LCD_DisplayString(1, "Press button to start");
			WELCOME_STATE = displayWait;
			break;
			
		case displayWait:
			if(!button)
			{
				WELCOME_STATE = displayWait;
			}
			else 
			{
				WELCOME_STATE = displayGameStart;
			}
			break;
			
		case displayGameStart:
			start = 1;
			srand(z);
			z++;
			assign_Inputs(inc);
			LCD_init();
			LCD_DisplayString(1, "Game start.");
			WELCOME_STATE = displayGameStart;
			break;
			
		default:
			break;
			
		
		
	}
}

unsigned char store = 0;
unsigned char fail;
unsigned char win;

void read_input() //checks for user correct input
{
	button = ~PINA & 0x0F;
	PWM_on();
	set_PWM(0);
		if(button == 0x01)
		{
			store = 0;
			PORTB = button;
			if(store == arrayINPUT[j])
			{
				set_PWM(261.63);
				++j;
			}
			else
			{
				set_PWM(261.63);
				fail = 1;
			}
		}
		else if(button == 0x02)
		{
			store = 1;
			PORTB = button;
			if(store == arrayINPUT[j])
			{
				set_PWM(329.63);
				++j;
			}
			else
			{
				set_PWM(329.63);
				fail = 1;
			}
		}
		else if(button == 0x04)
		{
			store = 2;
			PORTB = button;
			if(store == arrayINPUT[j])
			{
				set_PWM(392.00);
				++j;
			}
			else
			{
				set_PWM(392.00);
				fail = 1;
			}
		}
		else if(button == 0x08)
		{
			store = 3;
			PORTB = button;
			if(store == arrayINPUT[j])
			{
				set_PWM(493.88);
				++j;
			}
			else
			{
				set_PWM(493.88);
				fail = 1;
			}
		}
		
		if(j == i)
		{
			wait_input = 0;
			i = 0;
			inc++;
			
			if(!(inc == 9))
			{
				assign_Inputs(inc);
			}

			if(inc == 9)
			{
				win = 1;
			}
			else
			{
				LCD_init();
				LCD_DisplayString(1, "Score: ");
				LCD_WriteData(inc + '0');
			}
		}
	
	
}


double arrayRAND[9] = {};
unsigned char tmpB;
unsigned char tmpRAND;
//unsigned char arrayTmp_RAND = 0x00;

enum LIGHT_STATE
{
	init_write,
	BLUE,
	YELLOW,
	GREEN,
	RED,
	OFF,
} LIGHT_STATES;



void start_game()
{
	switch(LIGHT_STATES)
	{
		case init_write:
			PORTB = 0x00;
			if(i<(inc+1))
			{
				if ((arrayINPUT[i] == 0))
				{
					LIGHT_STATES = BLUE;
					i++;
					break;
				}
				else if ((arrayINPUT[i] == 1))
				{
					LIGHT_STATES = YELLOW;
					i++;
					break;
				}
				else if ((arrayINPUT[i] == 2))
				{
					LIGHT_STATES = GREEN;
					i++;
					break;
				}
				else if ((arrayINPUT[i] == 3))
				{
					LIGHT_STATES = RED;
					i++;
					break;
				}
			}
			else if(i == (inc+1))
			{
				LIGHT_STATES = OFF;
				break;
			}
			break;
		
		case BLUE:
			set_PWM(261.63);
			PORTB = 0x01;
			LIGHT_STATES = init_write;
			break;
		
		case YELLOW:
			set_PWM(329.63);
			PORTB = 0x02;
			LIGHT_STATES = init_write;
			break;
		
		case GREEN:
			set_PWM(392.00);
			PORTB = 0x04;
			LIGHT_STATES = init_write;
			break;
		
		case RED:
			set_PWM(493.88);
			PORTB = 0x08;
			LIGHT_STATES = init_write;
			break;
		
		case OFF:
			set_PWM(0);
			PORTB = 0x00;
			wait_input = 1;
			j = 0;
			LIGHT_STATES = init_write;
			break;
	}
}

enum FAIL_STATE
{
	init_fail,
	wait,
} FAIL_STATES;
	
unsigned char k;
	
void fail_screen()
{
	switch(FAIL_STATES)
	{
	
		case init_fail:
			if(fail)
			{
				set_PWM(0);
				k = 0;
				FAIL_STATES = wait;
				i = 0;
				LCD_init();
				LCD_DisplayString(1, "Game over. Final score: ");
				LCD_WriteData(inc + '0');
				j = 0;
				inc = 0;
			}
			break;
			
		case wait:
			if(k < 6)
			{
				FAIL_STATES = wait;
				++k;
			}
			
			if(!(k < 6))
			{
				start = 0;
				fail = 0;
				FAIL_STATES = init_fail;
			}
			break;
	}
	
}

enum WIN_STATE
{
	init_win,
	wait_win,
} WIN_STATES;

void win_screen()
{
	switch(WIN_STATES)
	{
		
		case init_win:
		if(win)
		{
			set_PWM(0);
			k = 0;
			WIN_STATES = wait_win;
			i = 0;
			LCD_init();
			LCD_DisplayString(1, "You win! Final  score: ");
			LCD_WriteData(inc + '0');
			j = 0;
			inc = 0;
		}
		break;
		
		case wait_win:
		if(k < 6)
		{
			WIN_STATES = wait_win;
			++k;
		}
		
		if(!(k < 6))
		{
			start = 0;
			fail = 0;
			win = 0;
			WIN_STATES = init_win;
		}
		break;
	}
	
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	unsigned long blink_elapsedTime = 200;
	unsigned long start_elapsedTime = 200;
	unsigned long input_elapsedTime = 200;
	unsigned long fail_elapsedTime = 500;
	unsigned long win_elapsedTime = 500;
	const unsigned long timerPeriod = 100;
	TimerSet(timerPeriod);
	TimerOn();
	PWM_on();
	i = 0;
	j = 0;
	start = 0;
	button = 0;
	wait_input = 0;
	inc = 0;
	fail = 0;
	win = 0;
	z = 1;
	LIGHT_STATES = init_write;
	WELCOME_STATE = init_welcome;
	FAIL_STATES = init_fail;
	WIN_STATES = init_win;

	while(1)
	{
		if(blink_elapsedTime >=200)
		{
			if(!start && !fail && !win)
			{
				welcome_screen();
			}
			blink_elapsedTime = 0;
		}
		if(start_elapsedTime >=500)
		{
			if(start && !wait_input && !fail && !win)
			{
				PWM_on();
				set_PWM(0);
				start_game();
			}
			start_elapsedTime = 0;
		}
		if(input_elapsedTime >=200)
		{
			if(start && wait_input && !fail && !win)
			{
				read_input();
			}
			input_elapsedTime = 0;
		}
		if(fail_elapsedTime >= 500)
		{
			if(fail)
			{
				fail_screen();
				WELCOME_STATE = init_welcome;
				LIGHT_STATES = init_write;
				inc = 0;
				i = 0;
				j = 0;
				wait_input = 0;
			}
			fail_elapsedTime = 0;
		}
		if(win_elapsedTime >= 500)
		{
			if(win)
			{
				win_screen();
				WELCOME_STATE = init_welcome;
				LIGHT_STATES = init_write;
				inc = 0;
				i = 0;
				j = 0;
				wait_input = 0;
			}
			win_elapsedTime = 0;
		}
		while (!TimerFlag)
		{
			
		}
		TimerFlag = 0;
		fail_elapsedTime += timerPeriod;
		blink_elapsedTime += timerPeriod;
		start_elapsedTime += timerPeriod;
		input_elapsedTime += timerPeriod;
		win_elapsedTime += timerPeriod;
	}
}