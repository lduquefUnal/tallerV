/**
 ******************************************************************************
 * @file           : main.c
 * @author         : namontoy@unal.edu.co
 * @brief          : Here starts the magic!!
 ******************************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <stm32f4xx.h>

#define RCC_BASE_ADDRESS                            0x40023800UL
#define RCC_AHB1ENR_OFFSET							0x30
#define RCC_AHB1ENR									(RCC_BASE_ADDRESS+RCC_AHB1ENR_OFFSET)

#define GPIOA_BASE_ADDRESS							0x40020000UL
#define GPIO_MODE_REG_OFFSET						0x00
#define GPIO_MODE_ODR_OFFSET						0x14
#define GPIO_MODE_REG								(GPIOA_BASE_ADDRESS+GPIO_MODE_REG_OFFSET)
#define GPIO_OPD_REG								(GPIOA_BASE_ADDRESS+GPIO_MODE_ODR_OFFSET)

//definir la dirección del puerto C
#define GPIOC_BASE_ADDRESS 							0x40020800UL
#define GPIO_PUPD_REG_OFFSET 						0x0C
#define GPIOC_ID_REG_OFFSET							0x10
#define GPIOC_ID_REG								(GPIOC_BASE_ADDRESS + GPIOC_ID_REG_OFFSET)
#define GPIOC_MODE_REG								(GPIOC_BASE_ADDRESS + GPIO_MODE_REG_OFFSET)
#define GPIOC_PUPD_REG								(GPIOC_BASE_ADDRESS + GPIO_PUPD_REG_OFFSET) //PULL UP PULL DOWN
// Definicion de las cabeceras de las funciones del main
extern void configMagic(void);

/*
 * The main function, where everything happens.
 */
int main(void)
{
	/* Configuramos la magia! */
	configMagic();
	printf("pruebas gpio No Hal \n");

	uint32_t *registerAHB1enb = (uint32_t *)RCC_AHB1ENR;
	*registerAHB1enb |= (1 << 2); // activa señal de reloj puerto GPIOc
	*registerAHB1enb |= (1 << 0); // activa señal de reloj puerto GPIOA

	uint32_t *registerGPIO_MODE = (uint32_t *)GPIO_MODE_REG;
	*registerGPIO_MODE |= (1<<10);

	uint32_t *registerGPIO_ODR = (uint32_t *)GPIO_OPD_REG;
	*registerGPIO_ODR |= (1 << 5) ; //led green set

	// configuración P13
	*registerAHB1enb |= (1 << 2);// activa la señal de reloj para el puerto GPIOC

	// manipulación de registro GPIOC
	uint32_t *registerGPIOC_MODE = (uint32_t *)GPIOC_MODE_REG;
	*registerGPIOC_MODE &= ~(0b11 << 26);

	uint32_t *registerGPIOC_PUPD = (uint32_t *)GPIOC_PUPD_REG;
	*registerGPIOC_PUPD &=  ~(0b11 << 26);

	uint32_t *registerGPIOC_ID = (uint32_t *)GPIOC_ID_REG;
	uint32_t idr_Gpioc_Value = *registerGPIOC_ID;
	uint32_t pinc13_Value = 0;

	while (1) {
		idr_Gpioc_Value = *registerGPIOC_ID;
//		extraemos el valro del pin 13
		pinc13_Value = (idr_Gpioc_Value >> 13);
		if(pinc13_Value==1){
//			encender led
			*registerGPIO_ODR |= (1<<5);
		}else{
//			apagar led
			*registerGPIO_ODR &= ~(1<<5) ;
		}
	}

	return 0;
}

