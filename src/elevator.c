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
			if(e->selectedFloor == floor) break;
			e->selectedFloor = floor;
			e->st = CLOSE_DOOR;
			e->ev = CLOSING;
			break;
		}
		default: {
			if(e->selectedFloor == floor) break;
			enqueue(e->reservation, floor);
			break;
		}
	}
}

char* getInfoClosing(elevator* e) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "Closing the door at floor %d...\0", e->floor);
	return buf;
}

char* getInfoOpening(elevator* e) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "Opening the door at floor %d...\0", e->floor);
	return buf;
}

char* getInfoLiftFloor(char* s) {
	char *buf = malloc(sizeof(char)*1024);
	sprintf(buf, "The floor is %s...\0", s);
	return buf;
}
