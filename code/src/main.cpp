#include "Arduino.h"

enum pins{
	PA4 = 0,
	PA5 = 1,
	PA6 = 2,
	PA7 = 3,
	PB3 = 4,
	PB2 = 5,
	PB1 = 6,
	PB0 = 7,
	YELLOW = 8,
	GREEN  = 10,
	RED    = 9
};

void setup(){
	pinMode(RED, OUTPUT);
	pinMode(YELLOW, OUTPUT);
	pinMode(GREEN, OUTPUT);
}

void loop(){
	digitalWrite(GREEN, HIGH);
	delay(500);
	digitalWrite(YELLOW, HIGH);
	delay(500);
	digitalWrite(RED, HIGH);
	delay(500);
	digitalWrite(RED, LOW);
	digitalWrite(YELLOW, LOW);
	digitalWrite(GREEN, LOW);
	delay(500);

}
