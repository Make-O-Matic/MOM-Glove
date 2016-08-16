#include <Arduino.h>
#include <Servo.h>

Servo serv;

const int led = 13;
const int servo = 10;

int serial_putchar(char c, FILE* f);
void setup();
void loop();

int serial_putchar(char c, FILE* f)
{
	if (c == '\n') serial_putchar('\r', f);
	return Serial.write(c) == 1? 0 : 1;
}

FILE serial_stdout;

void setup()
{
	pinMode(led, OUTPUT);
	
	Serial.begin(115200);
	fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
	stdout = &serial_stdout;
	
	serv.attach(servo);
	serv.write(90);
}

void loop()
{
	printf("Arduino-Library-Test by Daniel A. Maierhofer 2014\n");
	digitalWrite(led, HIGH);
	delay(200);
	digitalWrite(led, LOW);
	delay(300);
}
