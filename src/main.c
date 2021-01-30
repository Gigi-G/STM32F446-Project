/*
 * main.c
 *
 *  Created on: 29 gen 2021
 *      Author: gigi-g
 */

#include "stm32_unict_lib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

typedef struct {
	uint8_t moving;
	uint8_t selectedFloor;
	uint8_t floor;
	uint8_t direction;
	uint16_t speed;
} Elevator;

typedef enum {
	SELECT,
	SETUP,
	CLOSE_DOOR,
	OPEN_DOOR,
	MOVING
} State;

typedef enum {
	NONE
} Event;

queue *reservation;
Elevator elevator;
State stato = SELECT;
Event evento = NONE;

int counter = 0;
char hyphen = '-';
char buffer[5];

void start() {
	//Elevator
	elevator.moving = 0;
	elevator.selectedFloor = 1;
	elevator.floor = 1;
	elevator.speed = 10;
	elevator.direction = 1;

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
	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 8400, 1000); //Update IRQ ogni 100ms
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_init(TIM3);
	TIM_config_timebase(TIM3, 8400, 10000); //Update IRQ ogni 1s -> questo timer si setta in base alla velocità dell'ascensore
	TIM_enable_irq(TIM3, IRQ_UPDATE);

	//ADC initialize
	ADC_init(ADC1, ADC_RES_8, ADC_ALIGN_RIGHT);
	ADC_channel_config(ADC1, GPIOC, 0, 10);
	ADC_on(ADC1);

	//Display
	DISPLAY_init();
	sprintf(buffer, "%4d\0", elevator.floor);
	DISPLAY_puts(0, buffer);

	//USART
	CONSOLE_init();

	//RESERVATION initialize
	reservation = malloc(sizeof(queue));
	initialize(reservation);
}

void setRedLed(int i) {
	GPIOB->ODR = (GPIOB->ODR & ~(uint32_t)(1)) | (i);
}

void setGreenLed(int i) {
	GPIOC->ODR = (GPIOC->ODR & ~(uint32_t)(8)) | (i << 3);
}

void setYellowLed(int i) {
	GPIOC->ODR = (GPIOC->ODR & ~(uint32_t)(4)) | (i << 2);
}

int main() {
	start();
	for(;;) {
		switch(stato) {
			case SETUP: {
				ADC_sample_channel(ADC1, 10);
				ADC_start(ADC1);
				while(!ADC_completed(ADC1)) {}
				elevator.speed = (ADC_read(ADC1) * 6)/255 + 4;
				if(elevator.speed != 10) sprintf(buffer, "%3d%d\0", 0, elevator.speed);
				else sprintf(buffer, "%4d\0", elevator.speed);
				DISPLAY_dp(2, 1);
				DISPLAY_puts(0, buffer);
				break;
			}
			case SELECT:  {
				DISPLAY_dp(2, 0);
				sprintf(buffer, "%4d\0", elevator.floor);
				DISPLAY_puts(0, buffer);
				break;
			}
		}
	}
}

void selectFloor(uint8_t floor) {
	switch(stato) {
		case SELECT: {
			if(elevator.selectedFloor == floor) break;
			elevator.selectedFloor = floor;
			stato = CLOSE_DOOR;
			TIM_on(TIM2);
			break;
		}
		default: {
			if(elevator.selectedFloor == floor) break;
			enqueue(reservation, floor);
			break;
		}
	}
}

void EXTI15_10_IRQHandler(void) {
	if(EXTI_isset(EXTI10)) { //Button X = Primo Piano
		selectFloor(1);
		EXTI_clear(EXTI10);
	}
}

void EXTI4_IRQHandler(void) {
	if(EXTI_isset(EXTI4)) { //Button Y = Seconda Piano
		selectFloor(2);
		EXTI_clear(EXTI4);
	}
}

void EXTI9_5_IRQHandler(void) {
	if(EXTI_isset(EXTI5)) { //Button Z = Terzo Piano
		selectFloor(3);
		EXTI_clear(EXTI5);
	}
	if(EXTI_isset(EXTI6)) { //Button T
		if(stato == SELECT) {
			stato = SETUP;
		}
		else if(stato == SETUP) {
			stato = SELECT;
			TIM_off(TIM3);
			TIM_config_timebase(TIM3, 8400, 1000*elevator.speed);
		}
		EXTI_clear(EXTI6);
	}
}

void TIM2_IRQHandler(void) {
	if(TIM_update_check(TIM2)) {
		switch(stato) {
			case CLOSE_DOOR: {
				counter+=100;
				if(counter == 1500) {
					counter = 0;
					if(elevator.floor > elevator.selectedFloor) elevator.direction = -1;
					else elevator.direction = 1;
					elevator.moving = 1;
					stato = MOVING;
					TIM_set(TIM3, 0);
					TIM_on(TIM3);
					TIM_set(TIM2,0);
					setRedLed(0);
				}
				else if(counter%200 == 0) {
					GPIO_toggle(GPIOB, 0);
				}
				break;
			}
			case OPEN_DOOR: {
				counter+=100;
				if(counter == 1500) {
					counter = 0;
					if(isempty(reservation)) {
						elevator.moving = 0;
						stato = SELECT;
						TIM_off(TIM3);
						TIM_off(TIM2);
						setGreenLed(0);
					}
					else {
						elevator.selectedFloor = dequeue(reservation);
						stato = CLOSE_DOOR;
						TIM_off(TIM3);
						setGreenLed(0);
					}
				}
				else if(counter%200 == 0) {
					GPIO_toggle(GPIOC, 3);
				}
				break;
			}
			case MOVING: {
				counter+=100;
				if(counter%200 == 0) {
					GPIO_toggle(GPIOC, 2);
				}
				break;
			}
		}
		TIM_update_clear(TIM2);
	}
}

void TIM3_IRQHandler(void) {
	if(TIM_update_check(TIM3)) {
		if(stato == MOVING) {
			if(elevator.selectedFloor == elevator.floor) {
				TIM_off(TIM2);
				counter = 0;
				elevator.floor = (char)(elevator.selectedFloor);
				setYellowLed(0);
				sprintf(buffer, "%4d\0", elevator.floor);
				DISPLAY_puts(0, buffer);
				hyphen = '-';
				stato = OPEN_DOOR;
				TIM_set(TIM2, 0);
				TIM_on(TIM2);
			} else {
				if(elevator.direction == 1) {
					if(hyphen == '-') {
						sprintf(buffer, "%3d%c\0", elevator.floor, hyphen);
						hyphen = ' ';
					}
					else {
						elevator.floor += elevator.direction;
						sprintf(buffer, "%4d\0", elevator.floor);
						hyphen = '-';
					}
				}
				else {
					if(hyphen == '-') {
						elevator.floor += elevator.direction;
						sprintf(buffer, "%3d%c\0", elevator.floor, hyphen);
						hyphen = ' ';
					}
					else {
						sprintf(buffer, "%4d\0", elevator.floor);
						hyphen = '-';
					}
				}
				DISPLAY_puts(0, buffer);
			}
		}
		TIM_update_clear(TIM3);
	}
}
