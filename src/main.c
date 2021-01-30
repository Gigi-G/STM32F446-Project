/*
 * main.c
 *
 *  Created on: 29 gen 2021
 *      Author: gigi-g
 */

#include "stm32_unict_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include "elevator.h"

elevator *lift;
int counter = 0;
char hyphen = '-';
char buffer[5];

/**
 * Format log.
 *
 * @params	s is the string that will be show in the console.
 */
void printLog(char* s) {
	printf("\n-----------------------------------------------------------------------------------------\n");
	printf("%s\n", s);
	printf("-----------------------------------------------------------------------------------------\n\n");
}

/**
 * Initialize:
 * - ELEVATOR
 * - GPIO
 * - TIMERS:
 * 		- TIM2
 * 			- PRESCALER = 8400
 * 			- AUTORELOAD = 1000
 * 			- COUNTER CLOCK SET TO 0.1ms
 * 		- TIM3
 * 			- PRESCALER = 8400
 * 			- AUTORELOAD = 10000
 * 			- COUNTER CLOCK SET TO 0.1ms
 * - ADC
 * 		- ADC RESOLUTION = 8 bit
 * 		- ADC ALIGN = RIGHT ALIGN
 * 		- GPIO = GPIOC
 * 		- PIN = 0
 * 		- CHANNEL = 10
 * - DISPLAY
 * - USART
 */
void start() {
	//ELEVATOR initialize
	lift = malloc(sizeof(elevator));
	initializeElevator(lift);

	//GPIO initialize
	GPIO_init(GPIOB);
	GPIO_init(GPIOC);
	GPIO_config_output(GPIOB, 0);
	GPIO_config_output(GPIOC, 3);
	GPIO_config_output(GPIOC, 2);
	GPIO_config_EXTI(GPIOB, EXTI10);
	EXTI_enable(EXTI10, FALLING_EDGE);
	GPIO_config_EXTI(GPIOB, EXTI4);
	EXTI_enable(EXTI4 , FALLING_EDGE);
	GPIO_config_EXTI(GPIOB, EXTI5);
	EXTI_enable(EXTI5, FALLING_EDGE);
	GPIO_config_EXTI(GPIOB, EXTI6);
	EXTI_enable(EXTI6 , FALLING_EDGE);

	//TIMER initialize
	// TIM2 is used to flash leds during OPEN_DOOR -> GREEN LED, CLOSE_DOOR -> RED LED and MOVING -> YELLOW LED
	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 8400, 1000);
	TIM_enable_irq(TIM2, IRQ_UPDATE);

	// TIM3 is used for the speed of the elevator
	TIM_init(TIM3);
	TIM_config_timebase(TIM3, 8400, 10000);
	TIM_enable_irq(TIM3, IRQ_UPDATE);

	//ADC initialize
	ADC_init(ADC1, ADC_RES_8, ADC_ALIGN_RIGHT);
	ADC_channel_config(ADC1, GPIOC, 0, 10);
	ADC_on(ADC1);

	//DISPLAY
	DISPLAY_init();
	sprintf(buffer, "%4d", lift->floor);
	DISPLAY_puts(0, buffer);

	//USART
	CONSOLE_init();
	printLog("System is running...");
}

/**
 * Used for reservations events.
 */
void reservationEvent() {
	if(lift->res == NORESERVATION) {
		printLog(getInfoReservation(lift));
		lift->res = NONE;
	}
	else if(lift->res == EXISTRESERVATION) {
		printLog(getInfoReservation(lift));
		lift->res = NONE;
	}
	else if(lift->res == NEWRESERVATION) {
		printLog(getInfoReservation(lift));
		lift->res = NONE;
	}
}

/**
 * Used in setup mode and it sets the speed of the elevator.
 */
void setSpeed() {
	ADC_sample_channel(ADC1, 10);
	ADC_start(ADC1);
	while(!ADC_completed(ADC1)) {}
	lift->speed = (ADC_read(ADC1) * 6)/255 + 4;
	if(lift->speed != 10) sprintf(buffer, "%3d%d", 0, lift->speed);
	else sprintf(buffer, "%4d", lift->speed);
	DISPLAY_puts(0, buffer);
}

void setupCase() {
	if(lift->ev == ENTERSETUP) {
		printLog("System enter setup mode...");
		lift->ev = NONE;
		DISPLAY_dp(2, 1);
	}
	setSpeed();
}

void selectCase() {
	if(lift->ev == QUITSETUP) {
		printLog(getInfoSpeed(lift));
		printLog("System quit setup mode...");
		lift->ev = NONE;
		DISPLAY_dp(2, 0);
		sprintf(buffer, "%4d", lift->floor);
		DISPLAY_puts(0, buffer);
	}
}

void closeDoorCase() {
	if(lift->ev == CLOSING) {
		printLog(getInfoDestination(lift));
		printLog(getInfoClosing(lift));
		lift->ev = NONE;
		TIM_set(TIM2, 0);
		TIM_on(TIM2);
	}
}

void openDoorCase() {
	if(lift->ev == OPENING) {
		printLog(getInfoOpening(lift));
		lift->ev = NONE;
		TIM_set(TIM2, 0);
		TIM_on(TIM2);
	}
}

void movingCase() {
	if(lift->ev == LIFTFLOOR) {
		printLog(getInfoLiftFloor(buffer));
		lift->ev = NONE;
	}
}

int main() {
	start();
	for(;;) {
		switch(lift->st) {
			case SETUP: 		{setupCase(); break;}

			case SELECT: 		{selectCase(); break;}

			case CLOSE_DOOR:	{closeDoorCase(); break;}

			case OPEN_DOOR: 	{openDoorCase(); break;}

			case MOVING: 		{movingCase(); break;}
		}
		reservationEvent();
	}
}

void EXTI15_10_IRQHandler(void) {
	if(EXTI_isset(EXTI10)) { //Button X = First floor
		selectFloor(1, lift);
		EXTI_clear(EXTI10);
	}
}

void EXTI4_IRQHandler(void) {
	if(EXTI_isset(EXTI4)) { //Button Y = Second floor
		selectFloor(2, lift);
		EXTI_clear(EXTI4);
	}
}

void EXTI9_5_IRQHandler(void) {
	if(EXTI_isset(EXTI5)) { //Button Z = Third floor
		selectFloor(3, lift);
		EXTI_clear(EXTI5);
	}
	if(EXTI_isset(EXTI6)) { //Button T = ENTER/QUIT SETUP mode
		if(lift->st == SELECT) {
			lift->ev = ENTERSETUP;
			lift->st = SETUP;
		}
		else if(lift->st == SETUP) {
			lift->ev = QUITSETUP;
			lift->st = SELECT;
			TIM_config_timebase(TIM3, 8400, 1000*lift->speed);
		}
		EXTI_clear(EXTI6);
	}
}

void selectDirection() {
	if(lift->floor > lift->selectedFloor) lift->direction = -1;
	else lift->direction = 1;
}

void closeDoorTimeOut() {
	counter = 0;
	selectDirection();
	lift->st = MOVING;
	TIM_set(TIM3, 0);
	TIM_on(TIM3);
	TIM_set(TIM2,0);
	GPIO_write(GPIOB, 0, 0);
}

void closeDoorTIM2() {
	counter+=100;
	if(counter == 1500) {
		closeDoorTimeOut();
	}
	else if(counter%200 == 0) {
		GPIO_toggle(GPIOB, 0);
	}
}

void nextFloor() {
	if(isempty(lift->reservation)) {
		lift->st = SELECT;
	}
	else {
		lift->selectedFloor = dequeue(lift->reservation);
		lift->st = CLOSE_DOOR;
		lift->ev = CLOSING;
	}
}

void openDoorTimeOut() {
	counter = 0;
	TIM_off(TIM2);
	GPIO_write(GPIOC, 3, 0);
	nextFloor();
}

void openDoorTIM2() {
	counter+=100;
	if(counter == 1500) {
		openDoorTimeOut();
	}
	else if(counter%200 == 0) {
		GPIO_toggle(GPIOC, 3);
	}
}

void blinkingYellowLed() {
	counter+=100;
	if(counter%200 == 0) {
		GPIO_toggle(GPIOC, 2);
	}
}

void TIM2_IRQHandler(void) {
	if(TIM_update_check(TIM2)) {
		if(lift->st == CLOSE_DOOR) 		{closeDoorTIM2();}
		else if(lift->st == OPEN_DOOR)	{openDoorTIM2();}
		else if(lift->st == MOVING)		{blinkingYellowLed();}
		TIM_update_clear(TIM2);
	}
}

void TIM3_IRQHandler(void) {
	if(TIM_update_check(TIM3)) {
		if(lift->st == MOVING) {
			if(lift->selectedFloor == lift->floor) {
				TIM_off(TIM2);
				TIM_off(TIM3);
				counter = 0;
				lift->floor = (char)(lift->selectedFloor);
				GPIO_write(GPIOC, 2, 0);
				sprintf(buffer, "%4d", lift->floor);
				DISPLAY_puts(0, buffer);
				hyphen = '-';
				lift->st = OPEN_DOOR;
				lift->ev = OPENING;
			} else {
				if(lift->direction == 1) {
					if(hyphen == '-') {
						sprintf(buffer, "%3d%c", lift->floor, hyphen);
						hyphen = ' ';
					}
					else {
						lift->floor += lift->direction;
						sprintf(buffer, "%4d", lift->floor);
						hyphen = '-';
					}
				}
				else {
					if(hyphen == '-') {
						sprintf(buffer, "%3d%c", lift->floor-1, hyphen);
						hyphen = ' ';
					}
					else {
						lift->floor += lift->direction;
						sprintf(buffer, "%4d", lift->floor);
						hyphen = '-';
					}
				}
				lift->ev = LIFTFLOOR;
				DISPLAY_puts(0, buffer);
			}
		}
		TIM_update_clear(TIM3);
	}
}
