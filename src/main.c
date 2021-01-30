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

void printLog(char* s) {
	printf("\n-----------------------------------------------------------------------------------------\n");
	printf("%s\n", s);
	printf("-----------------------------------------------------------------------------------------\n");
}

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
	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 8400, 1000); //Update IRQ ogni 100ms
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_init(TIM3);
	TIM_config_timebase(TIM3, 8400, 10000); //Update IRQ ogni 1s
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
		switch(lift->st) {
			case SETUP: {
				ADC_sample_channel(ADC1, 10);
				ADC_start(ADC1);
				while(!ADC_completed(ADC1)) {}
				lift->speed = (ADC_read(ADC1) * 6)/255 + 4;
				if(lift->speed != 10) sprintf(buffer, "%3d%d", 0, lift->speed);
				else sprintf(buffer, "%4d", lift->speed);
				DISPLAY_dp(2, 1);
				DISPLAY_puts(0, buffer);
				break;
			}
			case SELECT:  {
				DISPLAY_dp(2, 0);
				sprintf(buffer, "%4d", lift->floor);
				DISPLAY_puts(0, buffer);
				break;
			}
			case CLOSE_DOOR: {
				if(lift->ev == CLOSING) {
					printLog(getInfoClosing(lift));
					lift->ev = NONE;
					TIM_set(TIM2, 0);
					TIM_on(TIM2);
				}
				break;
			}
			case OPEN_DOOR: {
				if(lift->ev == OPENING) {
					printLog(getInfoOpening(lift));
					lift->ev = NONE;
					TIM_set(TIM2, 0);
					TIM_on(TIM2);
				}
				break;
			}
			case MOVING: {
				if(lift->ev == LIFTFLOOR) {
					printLog(getInfoLiftFloor(buffer));
					lift->ev = NONE;
				}
			}
		}
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
	if(EXTI_isset(EXTI6)) { //Button T
		if(lift->st == SELECT) {
			lift->st = SETUP;
		}
		else if(lift->st == SETUP) {
			lift->st = SELECT;
			TIM_off(TIM3);
			TIM_config_timebase(TIM3, 8400, 1000*lift->speed);
		}
		EXTI_clear(EXTI6);
	}
}

void TIM2_IRQHandler(void) {
	if(TIM_update_check(TIM2)) {
		switch(lift->st) {
			case CLOSE_DOOR: {
				counter+=100;
				if(counter == 1500) {
					counter = 0;
					if(lift->floor > lift->selectedFloor) lift->direction = -1;
					else lift->direction = 1;
					lift->st = MOVING;
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
					TIM_off(TIM2);
					if(isempty(lift->reservation)) {
						lift->st = SELECT;
						setGreenLed(0);
					}
					else {
						lift->selectedFloor = dequeue(lift->reservation);
						lift->st = CLOSE_DOOR;
						lift->ev = CLOSING;
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
		if(lift->st == MOVING) {
			if(lift->selectedFloor == lift->floor) {
				TIM_off(TIM2);
				TIM_off(TIM3);
				counter = 0;
				lift->floor = (char)(lift->selectedFloor);
				setYellowLed(0);
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
						lift->floor += lift->direction;
						sprintf(buffer, "%3d%c", lift->floor, hyphen);
						hyphen = ' ';
					}
					else {
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
