/**
 ******************************************************************************
 * @file           : main.c
 * @author         : lduquef@unal.edu.co
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include "stm32_assert.h"
#include "timer_driver_hal.h"
#include "gpio_driver_hal.h"


GPIO_Handler_t userLed= {0}; // PinA5
GPIO_Handler_t userBtn = {0};
Timer_Handler_t reloj1 = {0};

uint8_t btnValue = {0};

int main(void)
{
	   /* Configuramos el pin */
	userLed.pGPIOx                          = GPIOA;
	userLed.pinConfig.GPIO_PinNumber        = PIN_5;
	userLed.pinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;

			/* Cargamos la configuración en los registros que gobiernan el puerto */
	gpio_Config(&userLed);
	gpio_WritePin(&userLed, SET);

	   /* Configuramos el pin */
	userBtn.pGPIOx                          = GPIOC;
	userBtn.pinConfig.GPIO_PinNumber        = PIN_13;
	userBtn.pinConfig.GPIO_PinMode          = GPIO_MODE_IN;
//	userBtn.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_PUSHPULL;
//	userBtn.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_MEDIUM;  no se necesitan por que es un in
	userBtn.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_PULLUP;
			/* Cargamos la configuración en los registros que gobiernan el puerto */
	gpio_Config(&userBtn);


	reloj1.pTIMx                       = TIM2;               // Seleccionamos TIM2
	reloj1.TIMx_Config.TIMx_Prescaler  = 16000;              // Genera incrementos de 1 ms  ; define el tiempo.
	reloj1.TIMx_Config.TIMx_Period     = 500;                // De la mano con el prescaler  ; cada 500 ms se lanza una interrupción
	reloj1.TIMx_Config.TIMx_mode       = TIMER_UP_COUNTER;   // Contador ascendente
	reloj1.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	// configuramos el Timer
	timer_Config(&reloj1);
	timer_SetState(&reloj1, TIMER_ON);
    /* Loop forever */
	while(1){
		btnValue = gpio_ReadPin(&userBtn);
		if(btnValue){
			reloj1.TIMx_Config.TIMx_Period = 1000;
			timer_SetState(&reloj1, TIMER_OFF);
			timer_Config(&reloj1) ; // subir las configuraciones
			timer_SetState(&reloj1, TIMER_ON);
		}
	}
}

void timer2_Callback(void){
	gpio_TogglePin(&userLed);
}

	/*
	 * Overwrite function
	 */
//void Timer2_Callback(void){
//	gpio_TogglePin(&userLed);
//}

	/*
	 * Esta función sirve para detectar problemas de parámetros
	 * incorrectos al momento de ejecutar un programa.
	 */
	void assert_failed(uint8_t *file, uint32_t line){
	    while(1){
	        // problems...
	    }
	}


