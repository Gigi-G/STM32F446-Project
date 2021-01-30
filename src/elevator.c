/*
 * elevator.c
 *
 *  Created on: 30 gen 2021
 *      Author: gigi-g
 */

#include "elevator.h"
#include <stdlib.h>

void initializeElevator(elevator *e) {
	e->direction = 1;
	e->ev = NONE;
	e->floor = 1;
	e->reservation = malloc(sizeof(queue));
	initialize(e->reservation);
	e->selectedFloor = 1;
	e->speed = 10;
	e->st = SELECT;
}

void selectFloor(uint8_t floor, elevator *e) {
	switch(e->st) {
		case SELECT: {
			if(e->selectedFloor == floor) {
				e->ev = NORESERVATION;
				break;
			}
			e->selectedFloor = floor;
			e->st = CLOSE_DOOR;
			e->ev = CLOSING;
			break;
		}
		default: {
			if(e->selectedFloor == floor) {
				e->ev = EXISTRESERVATION;
				break;
			}
			if(enqueue(e->reservation, floor) == 1)
				e->ev = NEWRESERVATION;
			else
				e->ev = EXISTRESERVATION;
			break;
		}
	}
}

char* getInfoClosing(elevator* e) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "Closing the door at floor %d...", e->floor);
	return buf;
}

char* getInfoOpening(elevator* e) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "Opening the door at floor %d...", e->floor);
	return buf;
}

char* getInfoDestination(elevator* e) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "The new destination is floor %d", e->selectedFloor);
	return buf;
}

char* getInfoLiftFloor(char* s) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "The floor is %s...", s);
	return buf;
}

char* getInfoSpeed(elevator *e) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "The selected speed is %.1f", e->speed/10.0);
	return buf;
}

char* getInfoReservation(elevator *e) {
	char *buf = malloc(sizeof(char)*1024);
	if(e->ev == NORESERVATION) {
		sprintf(buf, "You already are at floor %d", e->floor);
	}
	else if(e->ev == EXISTRESERVATION) {
		sprintf(buf, "The floor selected is a future destination, wait for it...");
	}
	else if(e->ev == NEWRESERVATION) {
		sprintf(buf, "The new reservation is for floor %d", readTail(e->reservation));
	}
	return buf;
}
