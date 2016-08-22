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
unsigned char last[ID_LENGTH];
uint8_t listcnt = 0;
uint8_t ledcnt = 0;
#define VIB_CNT 15
#define VIB_BIB 1
#define VIB_NR 14
uint8_t vibcnt = 0;
uint8_t vibr[] = {14,15,16,47,48,54,64,70,73};
#define KEY	6
#define SWITCH	7
#define CAPSENS	8

void process_rfid(uint8_t c)
{
	static int count = 0;
	if (c == 0x02)
	{
		count = 0;
	}
	else if (c == 0x03)
	{
		ledcnt = 0;
		if (memcmp(buffer, last, sizeof(last)) != 0)
		{
			memcpy(last, buffer, sizeof(last));
			drv.setWaveform(VIB_BIB, VIB_NR);
			drv.go();
			vibcnt = VIB_CNT;
		}
	}
	else if(count < 64)
	{
		buffer[count++] = c;
	}
}

inline void set_led(void)
{
	if (ledcnt < 10)
	{
		if (ledcnt == 0)
		{
			digitalWrite(LED_BUILTIN, HIGH);
		}
		ledcnt++;
	}
	else
	{
		digitalWrite(LED_BUILTIN, LOW);
	}
}

inline void set_vib(uint8_t vibnr)
{
	if (vibnr < sizeof(vibr))
	{
		drv.setWaveform(VIB_BIB, vibr[vibnr]);
		drv.go();
	}
}

inline void set_vib_2nd(void)
{
	if (vibcnt != 0)
	{
		vibcnt--;
		if (vibcnt == 0)
		{
			drv.setWaveform(VIB_BIB, VIB_NR);
			drv.go();
		}
	}
}

inline void print_bno(void)
{
	sensors_event_t event;
	bno.getEvent(&event);
	imu::Vector<3> vec = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

	Serial.print("{\"rfid\":\"");
	Serial.write(buffer,ID_LENGTH);
	Serial.print("\",\"imu\":{\"e\":{\"x\":");
	Serial.print(event.orientation.x, 4);
	Serial.print(",\"y\":");
	Serial.print(event.orientation.y, 4);
	Serial.print(",\"z\":");
	Serial.print(event.orientation.z, 4);
	Serial.print("},\"a\":{\"x\":");
	Serial.print(vec.x(), 4);
	Serial.print(",\"y\":");
	Serial.print(vec.y(), 4);
	Serial.print(",\"z\":");
	Serial.print(vec.z(), 4);
	Serial.print("}}");
}

inline void print_dig(void)
{
	Serial.print(",\"key\" :");
	Serial.print(digitalRead(KEY) ? '1' : '0');
	Serial.print(",\"switch\" :");
	Serial.print(digitalRead(SWITCH) ? '1' : '0');
	Serial.print(",\"capsens\" :");
	Serial.print(digitalRead(CAPSENS) ? '1' : '0');
	Serial.print("}\n");
}

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(KEY, INPUT_PULLUP);
	pinMode(SWITCH, INPUT_PULLUP);
	pinMode(CAPSENS, INPUT_PULLUP);
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
	drv.useERM();
	drv.selectLibrary(2);
	drv.setWaveform(VIB_BIB, VIB_NR);

	sei();
}

void loop()
{
	print_bno();
	print_dig();

	set_led();

	delay(20);

	while(SoftSerial.available())
	{
		process_rfid(SoftSerial.read());
	}
	if (Serial.available())
	{
		set_vib(Serial.read());
	}
	set_vib_2nd();
}
