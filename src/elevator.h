/*
 * elevator.h
 *
 *  Created on: 30 gen 2021
 *      Author: gigi-g
 *
 *  Description:
 *  	Elevator is defined in this file.
 */

#ifndef ELEVATOR_H_
#define ELEVATOR_H_

#include "queue.h"
#include <stdio.h>

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
	event res;
	uint8_t selectedFloor;
	uint8_t floor;
	uint8_t direction;
	uint16_t speed;
	queue *reservation;
};

/**
 * @struct elevator
 * @brief This structure is a definition of an elevator
 * @var elevator::st
 * Member st contains the state of the elevator: SELECT, SETUP, CLOSE_DOOR, OPEN_DOOR and MOVING
 * @var elevator::ev
 * Member ev contains an elevator event: CLOSING, OPENING, LIFTFLOOR, ENTERSETUP, QUITSETUP and NONE
 * @var elevator::res
 * Member res contains a reservation event: NEWRESERVATION, EXISTRESERVATION, NORESERVATION and NONE
 * @var elevator::selectedFloor
 * Member selectedFloor contains the floor to reach
 * @var elevator::floor
 * Member floor contains the current floor
 * @var elevator::direction
 * Member direction contains 1 if the elevator go up and -1 if the elevator go down
 * @var elevator::speed
 * Member speed contains the speed of the elevator
 * @var elevator::reservation
 * Member reservation is a queue for reservation
 */
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
