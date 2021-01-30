/*
 * elevator.h
 *
 *  Created on: 30 gen 2021
 *      Author: gigi-g
 */

#ifndef ELEVATOR_H_
#define ELEVATOR_H_

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum {
	SELECT,
	SETUP,
	CLOSE_DOOR,
	OPEN_DOOR,
	MOVING
} state;

typedef enum event {
	CLOSING,
	OPENING,
	LIFTFLOOR,
	ENTERSETUP,
	QUITSETUP,
	NEWRESERVATION,
	EXISTRESERVATION,
	NORESERVATION,
	NONE
} event;

struct elevator {
	state st;
	event ev;
	uint8_t selectedFloor;
	uint8_t floor;
	uint8_t direction;
	uint16_t speed;
	queue *reservation;
};
typedef struct elevator elevator;

void initializeElevator(elevator *e);

void selectFloor(uint8_t floor, elevator *e);

char* getInfoClosing(elevator* e);

char* getInfoOpening(elevator* e);

char* getInfoDestination(elevator *e);

char* getInfoLiftFloor(char* s);

char* getInfoSpeed(elevator *e);

char* getInfoReservation(elevator *e);

#endif /* ELEVATOR_H_ */
