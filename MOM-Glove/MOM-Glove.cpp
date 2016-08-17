#include <Arduino.h>
#include <SoftwareSerial.h>
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

SoftwareSerial SoftSerial(2, 3);
unsigned char buffer[64];
#define ID_LENGTH 12
unsigned char list[10][ID_LENGTH];
uint8_t listcnt = 0;
uint8_t ledcnt = 0;


void setup()
{
	pinMode(13, OUTPUT);
	Serial.begin(115200);
	SoftSerial.begin(9600);
	if(!bno.begin())
	{
		//uart_puts("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
		while(1);
	}
	delay(1000);
	bno.setExtCrystalUse(true);
	
	drv.begin();

	sei();
}

void loop()
{
	//sensors_event_t event;
	//bno.getEvent(&event);

	//Serial.print(event.orientation.x, 4);
	//Serial.print(";");
	//Serial.print(event.orientation.y, 4);
	//Serial.print(";");
	//Serial.println(event.orientation.z, 4);

	if (ledcnt < 10)
	{
		if (ledcnt == 0)
		{
			digitalWrite(13, HIGH);
		}
		ledcnt++;
	}
	else
	{
		digitalWrite(13, LOW);
	}

	delay(10);

	if (SoftSerial.available())
	{
		static int count = 0;
		while(SoftSerial.available())
		{
			uint8_t c = SoftSerial.read();
			if (c == 0x02)
			{
				count = 0;
			}
			else if (c == 0x03)
			{
				Serial.write(buffer,count);
				Serial.write('\n');
				ledcnt = 0;
				bool match = false;
				for (int i = 0; i < listcnt; i++)
				{
					if (memcmp(buffer, list[i], sizeof(list[0])) == 0)
					{
						match = true;
						break;
					}
				}
				if (! match && listcnt < 10)
				{
					memcpy(list[listcnt++], buffer, sizeof(list[0]));
					drv.go();
				}
			}
			else
			{
				buffer[count++]=c;
			}
			if(count == 64)
				break;
		}
	}
	if (Serial.available())
		SoftSerial.write(Serial.read());
}
