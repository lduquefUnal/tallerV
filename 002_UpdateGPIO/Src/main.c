/**
 ******************************************************************************
 * @file           : main.c
 * @author         : lduquef@unal.edu.co
 * @brief          : Basic project
 ******************************************************************************

 ******************************************************************************
 */

#include <stdint.h>
#include <stm32f4xx.h>
#
#include "stm32_assert.h"
#include "gpio_driver_hal.h"

GPIO_Handler_t userLed = {0};

int main(void)
{
    /* Configuramos el pin */
    userLed.pGPIOx                             = GPIOA;
    userLed.pinConfig.GPIO_PinNumber           = PIN_5;
    userLed.pinConfig.GPIO_PinMode             = GPIO_MODE_OUT;
    userLed.pinConfig.GPIO_PinOutputType       = GPIO_OTYPE_PUSHPULL;
    userLed.pinConfig.GPIO_PinOutputSpeed      = GPIO_OSPEED_MEDIUM;
    userLed.pinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;

    /* Cargamos la configuración en los registros que gobiernan el puerto */
    gpio_Config(&userLed);

    gpio_WritePin(&userLed, SET);

    while(1){
    	gpio_TogglePin(&userLed);
        for(uint32_t j = 0; j < 200000; j++){
            NOP();
        }
    }
}

void assert_failed(uint8_t* file,uint32_t line){
	while(1){

	}
}
