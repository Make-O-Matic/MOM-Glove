#include <Arduino.h>
#include "Adafruit_BNO055.h"
#include "Adafruit_DRV2605.h"
extern "C" {
	//#include "pfleury/uart.h"
};

FUSES =
{
	.low = 0xFF,	//0xFF
	.high = FUSE_SPIEN & FUSE_BOOTSZ0 & FUSE_BOOTRST,	//0xDC
	.extended = FUSE_BODLEVEL1	//0xFD
};
LOCKBITS = LB_MODE_3;	//0xFC

#define UART_BAUD_RATE	115200
#define FILTER_LOG2		4		// power of two

Adafruit_BNO055 bno = Adafruit_BNO055(55);
Adafruit_DRV2605 drv = Adafruit_DRV2605();

void setup()
{
	Serial.begin(115200);
	if(!bno.begin())
	{
		//uart_puts("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
		while(1);
	}
	delay(1000);
	bno.setExtCrystalUse(true);
	
	drv.begin();
	drv.go();

	sei();
}

void loop()
{
	sensors_event_t event;
	bno.getEvent(&event);

	Serial.print(event.orientation.x, 4);
	Serial.print(";");
	Serial.print(event.orientation.y, 4);
	Serial.print(";");
	Serial.println(event.orientation.z, 4);
	delay(10);
}
