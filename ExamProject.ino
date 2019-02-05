#include <avr/pgmspace.h>
#include "ExamProject.h"
#include "Interpreter.h"
#include "Data.h"

void setup()
{
	pinMode(OUTPIN, OUTPUT);
	setupTimer2();
	interpreterSetup();
	Serial.begin(9600);
  runCommands();
}

void loop(){}

void printByte(uint8_t data)
{
	for (unsigned int mask = 0x80; mask >= 0x01; mask >>= 1)
	{
		if (data & mask)
			Serial.print(1);
		else
			Serial.print(0);
	}
	Serial.print(" ");
}
