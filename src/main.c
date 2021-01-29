#include "stm32_unict_lib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void start() {
	//Random
	srand(time(0));

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
	TIM_config_timebase(TIM2, 8400, 100); //Update IRQ ogni 10ms
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_on(TIM2);
	TIM_init(TIM3);
	TIM_config_timebase(TIM3, 8400, 5000); //Update IRQ ogni 500ms
	TIM_enable_irq(TIM3, IRQ_UPDATE);
	TIM_on(TIM3);

	//ADC initialize
	ADC_init(ADC1, ADC_RES_12, ADC_ALIGN_RIGHT);
	ADC_channel_config(ADC1, GPIOC, 0, 10);
	ADC_channel_config(ADC1, GPIOC, 1, 11);
	ADC_on(ADC1);

	//Display
	DISPLAY_init();

	//USART
	CONSOLE_init();
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

typedef enum {
	RUN,
	SETUP
} State;

State stato = RUN;

int main() {
	start();
	for(;;) {

	}
}

void EXTI15_10_IRQHandler(void) {
	if(EXTI_isset(EXTI10)) { //Button X
		if(stato == RUN) {

		}
		else if(stato == SETUP) {

		}
		EXTI_clear(EXTI10);
	}
}

void EXTI4_IRQHandler(void) {
	if(EXTI_isset(EXTI4)) { //Button Y
		if(stato == SETUP) {

		}
		EXTI_clear(EXTI4);
	}
}

void EXTI9_5_IRQHandler(void) {
	if(EXTI_isset(EXTI5)) { //Button Z

		EXTI_clear(EXTI5);
	}
	if(EXTI_isset(EXTI6)) { //Button T

		EXTI_clear(EXTI6);
	}
}

void TIM2_IRQHandler(void) {
	if(TIM_update_check(TIM2)) {

		TIM_update_clear(TIM2);
	}
}

void TIM3_IRQHandler(void) {
	if(TIM_update_check(TIM3)) {
		if(stato == SETUP) {

		}
		else if(stato == RUN) {

		}
		TIM_update_clear(TIM3);
	}
}
