#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

inline void start_adc(short channel){
	ADC0.MUXPOS = channel;
	ADC0.COMMAND = 1;
}

int read_adc(void){
	while(ADC0.COMMAND); //don't block if ADC is not running
	return ADC0.RES;
}


void init(){
	/* LEDs */
	PORTA.DIR = 0x0E; // LED outputs for status
	/* PWM */
	PORTB.DIR = 0x05; // PWM outputs
	TCA0.SINGLE.PER = 1000;   // 20kHz
	TCA0.SINGLE.CMP0 = 250;   // Buck
	TCA0.SINGLE.CMP2 = 500;   // Boost
	TCA0.SINGLE.CTRLB = 0x53; //single-slope PWM, outputs 0 and 2
	TCA0.SINGLE.CTRLA = 1; // enable the timer
	CPU_CCP = 0xD8; // Allow changing the clock
	CLKCTRL.MCLKCTRLB = 0; // No prescaling, run at 20MHz
	/* ANALOG INPUTS */
	// Disable digital buffer for analog inputs
	PORTA.PIN4CTRL = 0x04; // Voltage In
	PORTA.PIN5CTRL = 0x04; // Voltage Out
	PORTA.PIN6CTRL = 0x04; // Current
	PORTA.PIN7CTRL = 0x04; // Aux
	PORTB.PIN1CTRL = 0x04; // USB Negotiated voltage
	VREF.CTRLA = VREF_ADC0REFSEL_2V5_gc;
	ADC0.CTRLC = 0x43;     // 5pF sample cap, internal reference, 1.25MHz ADC clock
	ADC0.SAMPCTRL = 0x02;  // Extend sample time to 4 clock cycles
	ADC0.CTRLA = 0x01;     // enabled, 10-bit resolution
}

int main(){
	init();
	while(1){
		start_adc(4);
		TCA0.SINGLE.CMP2 = read_adc();
		/*
		PORTA.OUTSET = 2;
		_delay_ms(500);
		PORTA.OUTSET = 4;
		_delay_ms(500);
		PORTA.OUTSET = 8;
		_delay_ms(500);
		PORTA.OUTCLR = 14;
		_delay_ms(500);
		*/
	}
}


