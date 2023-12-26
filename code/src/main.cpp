#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdfix.h>

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
	TCA0.SPLIT.LCMP0 = 120;    // Boost
	TCA0.SPLIT.LCMP2 = 100;    // Buck
	TCA0.SPLIT.HCMP0 = 128;  // LED
	TCA0.SPLIT.CTRLD = 1;	 // Split mode
	TCA0.SPLIT.CTRLB = 0x15; //outputs 0,2,3
	TCA0.SPLIT.CTRLA = 1; // enable the timer
	CPU_CCP = 0xD8; // Allow changing the clock
	CLKCTRL.MCLKCTRLB = 0; // No prescaling, run at 20MHz
	/* ANALOG INPUTS */
	// Disable digital buffer for analog inputs
	PORTA.PIN4CTRL = 0x04; // Voltage In
	PORTA.PIN5CTRL = 0x04; // Voltage Out
	PORTA.PIN6CTRL = 0x04; // Current
	PORTA.PIN7CTRL = 0x04; // Aux
	PORTB.PIN1CTRL = 0x04; // USB Negotiated voltage
	VREF.CTRLA = VREF_ADC0REFSEL_4V34_gc;
	ADC0.CTRLC = 0x43;     // 5pF sample cap, internal reference, 1.25MHz ADC clock
	ADC0.SAMPCTRL = 0x02;  // Extend sample time to 4 clock cycles
	ADC0.CTRLA = 0x01;     // enabled, 10-bit resolution
}

enum Adc
{
	adc_aux = 4,
	adc_iload = 5,
	adc_vin = 6,
	adc_vout = 7,
	adc_reqv = 10,
};

float parse_reqv(int reqv){
	if( reqv > 500 )
		return 5.5;
	if( reqv > 300 )
		return 8.0;
	return 11.0;
}

const float volts = 4.34 * 5.0 / 1024; //Scale ADC value to volts
int main(){
	init();
	uint8_t duty = 0;
	float setpoint = 5.0;
	PORTA.OUTSET = 2; // Turn on an LED so we know it's on
	while(1){
		start_adc(adc_vin);
		float vout = read_adc();
		start_adc(adc_vout);
		vout *= volts;
		if (vout < setpoint){
			++duty;
			_delay_us(100);
		} else if(duty > 0){
			--duty;
		}
		float vin = read_adc();
		start_adc(adc_reqv);
		vin *= volts;
		uint8_t maxd = (1 - vin / (setpoint+0.7)) * 256;
		if( duty > maxd){
			duty = maxd;
		}
		TCA0.SPLIT.LCMP0 = duty;
		setpoint = parse_reqv(read_adc());
		start_adc(adc_iload);
		if( setpoint > 6 ){
			PORTA.OUTSET = 4;
		} else {
			PORTA.OUTCLR = 4;
		}
		// Dislay brightness on green LED
		TCA0.SPLIT.HCMP0 = read_adc()/4;
	}
}
