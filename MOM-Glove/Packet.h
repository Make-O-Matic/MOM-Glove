#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

#define ID_LENGTH 12

typedef struct {
	uint8_t rfid[ID_LENGTH]; //<-- called "rfid" in #MOM-Trainer
	float ex, ey, ez; //<-- called "rotation.x, rotation.y, rotation.z" in #MOM-Trainer
	float ax, ay, az; //<-- called "acceleration.x, acceleration.y, acceleration.z" in #MOM-Trainer
	uint16_t graspa, graspb, graspc; //<-- called "grasp.sensorA, grasp.sensorB, grasp.sensorC" in #MOM-Trainer 
	uint8_t key : 1; //<-- called "interface.userInputButton" in #MOM-Trainer
	uint8_t wear : 1; //<-- called "interface.handIsInGlove" in #MOM-Trainer
	uint8_t lastnr : 1; //<-- called "calculated.isSameRFIDTag" in #MOM-Trainer
} Packet;

#endif // PACKET_H
