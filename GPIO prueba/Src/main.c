/**
 ******************************************************************************
 * @file           : main.c
 * @author         : namontoy@unal.edu.co
 * @brief          : Here starts the magic!!
 ******************************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "gpio_driver_hal.h"

GPIO_Handler_t pinLed5 = {0};
GPIO_Handler_t pinLed6 = {0};
GPIO_Handler_t pinLed7 = {0};
GPIO_Handler_t pinLed8 = {0};
GPIO_Handler_t userBtn = {0};
uint8_t contador = 0;
/*
 * The main function, where everything happens.
 */
int main(void)
{

//	configuramos los led para cada PIN
	pinLed5.pGPIOx 							= GPIOA;
	pinLed5.pinConfig.GPIO_PinNumber		= PIN_5;
	pinLed5.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	pinLed5.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	pinLed5.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	pinLed5.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	gpio_Config(&pinLed5);

	pinLed6.pGPIOx 							= GPIOA;
	pinLed6.pinConfig.GPIO_PinNumber		= PIN_6;
	pinLed6.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	pinLed6.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	pinLed6.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	pinLed6.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	gpio_Config(&pinLed6);
	pinLed7.pGPIOx 							= GPIOA;
	pinLed7.pinConfig.GPIO_PinNumber		= PIN_7;
	pinLed7.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	pinLed7.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	pinLed7.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	pinLed7.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	gpio_Config(&pinLed7);

	pinLed8.pGPIOx 							= GPIOA;
	pinLed8.pinConfig.GPIO_PinNumber		= PIN_8;
	pinLed8.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	pinLed8.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	pinLed8.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	pinLed8.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	gpio_Config(&pinLed8);

	userBtn.pGPIOx 							= GPIOC;
	userBtn.pinConfig.GPIO_PinNumber		= PIN_13;
	userBtn.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	userBtn.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	gpio_Config(&userBtn);

	gpio_WritePin( &pinLed5 , GPIO_PIN_SET );
	contador=1;
	while (1) {
	gpio_ReadPin(&userBtn);
	if(contador==1){
		gpio_WritePin(&pinLed5, GPIO_PIN_RESET);
		contador++;
	}else if(contador==2){
		gpio_WritePin(&pinLed6, GPIO_PIN_SET);
		contador++;
	}else if(contador==3){
		gpio_WritePin(&pinLed7, GPIO_PIN_SET);
		contador++;
	}else{
		gpio_WritePin(&pinLed5, GPIO_PIN_SET);
		gpio_WritePin(&pinLed6, GPIO_PIN_RESET);
		contador=1;
	}
	}

	return 0;
}

