/*
 * elevator.c
 *
 *  Created on: 30 gen 2021
 *      Author: gigi-g
 */

#include "elevator.h"
#include <stdlib.h>

/**
 * Initialize an elevator.
 *
 * @param	e is a pointer to a struct elevator
 */
void initializeElevator(elevator *e) {
	e->direction = 1;
	e->ev = NONE;
	e->res = NONE;
	e->floor = 1;
	e->reservation = malloc(sizeof(queue));
	initialize(e->reservation);
	e->selectedFloor = 1;
	e->speed = 10;
	e->st = SELECT;
}

/**
 * Permits to select a floor or reserve it, if it won't be a future destination.
 * If the selected floor is the current floor, it won't do nothing.
 *
 * @param	floor is the number of the floor
 * @param	e is a pointer to a struct elevator
 */
void selectFloor(uint8_t floor, elevator *e) {
	switch(e->st) {
		case SELECT: {
			if(e->selectedFloor == floor) {e->res = NORESERVATION; break;}
			e->selectedFloor = floor;
			e->st = CLOSE_DOOR;
			e->ev = CLOSING;
			break;
		}
		default: {
			if(e->selectedFloor == floor) {e->res = EXISTRESERVATION; break;}
			if(enqueue(e->reservation, floor) == 1)
				e->res = NEWRESERVATION;
			else
				e->res = EXISTRESERVATION;
			break;
		}
	}
}

/**
 * Provides information about the floor where the door is closing.
 *
 * @param	e is a pointer to a struct elevator
 * @return	buf is a string that contains the information
 */
char* getInfoClosing(elevator* e) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "Closing the door at floor %d...", e->floor);
	return buf;
}

/**
 * Provides information about the floor where the door is opening.
 *
 * @param	e is a pointer to a struct elevator
 * @return	buf is a string that contains the information
 */
char* getInfoOpening(elevator* e) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "Opening the door at floor %d...", e->floor);
	return buf;
}

/**
 * Provides information about the next destination.
 *
 * @param	e is a pointer to a struct elevator
 * @return	buf is a string that contains the information
 */
char* getInfoDestination(elevator* e) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "The new destination is floor %d", e->selectedFloor);
	return buf;
}

/**
 * Provides information about the floor reached during the movement.
 *
 * @param	e is a pointer to a struct elevator
 * @return	buf is a string that contains the information
 */
char* getInfoLiftFloor(char* s) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "The floor is %s...", s);
	return buf;
}

/**
 * Provides information about the speed of the elevator.
 *
 * @param	e is a pointer to a struct elevator
 * @return	buf is a string that contains the information
 */
char* getInfoSpeed(elevator *e) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "The selected speed is %.1f", e->speed/10.0);
	return buf;
}

/**
 * Provides information about the reservation operation.
 * - NORESERVATION means that the selected floor is equal to the current floor.
 * - EXISTRESERVATION means that the selected floor is a future destination.
 * - NEWRESRVATION means that the selected floor is correctly reserved.
 *
 * @param	e is a pointer to a struct elevator
 * @return	buf is a string that contains the information
 */
char* getInfoReservation(elevator *e) {
	char *buf = malloc(sizeof(char)*1024);
	if(e->res == NORESERVATION) {
		sprintf(buf, "You already are at floor %d", e->floor);
	}
	else if(e->res == EXISTRESERVATION) {
		sprintf(buf, "The floor selected is a future destination, wait for it...");
	}
	else if(e->res == NEWRESERVATION) {
		sprintf(buf, "The new reservation is for floor %d", readTail(e->reservation));
	}
	return buf;
}
