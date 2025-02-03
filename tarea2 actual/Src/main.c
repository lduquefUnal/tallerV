/**
 ******************************************************************************
 * @file           : main.c
 * @author         : lduquef@unal.edu.co
 * @brief          : tarea 2
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
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "gpio_driver_hal.h"
#include "stm32_assert.h"
#include "exti_driver_hal.h"
#include "timer_driver_hal.h"
#include "SysTickDriver.h"

#define DEBOUNCE_DELAY_MS 250
//Definimos un PIN de prueba
GPIO_Handler_t userLed	 	= {0}; // PinH1
GPIO_Handler_t userDis1 	= {0}; // PinA0
GPIO_Handler_t userDis2 	= {0}; // PinA1
GPIO_Handler_t userDis3 	= {0}; // PinA4
GPIO_Handler_t userDis4 	= {0}; // PinB0
GPIO_Handler_t userClk 		= {0}; // PinA12
GPIO_Handler_t userData 	= {0}; // PinC8
GPIO_Handler_t userSw 	    = {0}; // PinC9
GPIO_Handler_t userSegA     = {0};
GPIO_Handler_t userSegB     = {0};
GPIO_Handler_t userSegC     = {0};
GPIO_Handler_t userSegD     = {0};
GPIO_Handler_t userSegE     = {0};
GPIO_Handler_t userSegF     = {0};
GPIO_Handler_t userSegG     = {0};
GPIO_Handler_t ledRed       = {0};   // Pin C11
GPIO_Handler_t ledGreen     = {0}; // Pin D2
GPIO_Handler_t ledBlue      = {0};  // Pin C10

Timer_Handler_t blinkyTimer = {0}; //TIM2
Timer_Handler_t displayTimer = {0};
EXTI_Config_t extiClk	 	= {0};
EXTI_Config_t extiSw	 	= {0};

enum{
	unidades = 1,
	decenas,
	centenas,
	miles,
};
enum {
    LED_OFF = 0,           // Apagado
    LED_RED,               // Rojo
    LED_GREEN,             // Verde
    LED_BLUE,              // Azul
    LED_CYAN,              // Azul + Verde
    LED_MAGENTA,           // Azul + Rojo
    LED_YELLOW,            // Rojo + Verde
    LED_WHITE              // Rojo + Verde + Azul
} LED_Color_t;

typedef enum {
    FSM_IDLE,
    FSM_UPDATE_DISPLAY,
    FSM_ENCODER_UP,
    FSM_ENCODER_DOWN,
    FSM_CHANGE_LED_COLOR
} FSM_State_t;
FSM_State_t currentState = FSM_IDLE;

uint16_t estado = 0;
uint8_t estadoSw = 0;
uint8_t turno = 0;
uint8_t prevClock = 0;
uint8_t data = 0;
uint8_t clock = 0;
uint8_t sw = 0 ;
volatile uint8_t encoderEventFlag = 0;
volatile uint8_t swEventFlag;
uint8_t color=0;
uint8_t ms_delay = 4;
uint64_t time = 0;
// Se declaran las funciones auxiliares
void init_System(void);
void togglePin(void);
void displayNumber(uint8_t);
void set_LED_Color(uint8_t color);
volatile uint32_t lastDebounceTime = 0;  // Último tiempo en que se detectó un cambio
volatile uint8_t debounceInProgress = 0; // Bandera para evitar múltiples detecciones rápidas
/*
 * The main function, where everything happens.
 */
int main (void){

	init_System();
	displayNumber(8);
	config_SysTick_ms(1);
	while(1){
		gpio_WritePin(&userDis1, RESET);
		gpio_WritePin(&userDis2, RESET);
		gpio_WritePin(&userDis3, RESET);
		gpio_WritePin(&userDis4, RESET);
		gpio_WritePin(&userSegA, RESET);
		gpio_WritePin(&userSegB, RESET);
		gpio_WritePin(&userSegC, RESET);
		gpio_WritePin(&userSegD, RESET);
		gpio_WritePin(&userSegE, RESET);
		gpio_WritePin(&userSegF, RESET);
		gpio_WritePin(&userSegG, RESET);
		if (debounceInProgress && (getTicks_ms() - lastDebounceTime) > DEBOUNCE_DELAY_MS) {
		    debounceInProgress = 0;  // Permitir nuevas detecciones de pulsación
		}
		switch (currentState) {
			case FSM_IDLE:
				if (swEventFlag) {
					currentState = FSM_CHANGE_LED_COLOR;
				} else if (encoderEventFlag) {
					currentState = (data == 0) ? FSM_ENCODER_UP : FSM_ENCODER_DOWN;
				} else {
					currentState = FSM_UPDATE_DISPLAY;
				}
				break;

			case FSM_CHANGE_LED_COLOR:
				swEventFlag = 0;
				if (estadoSw < 9) {
					set_LED_Color(estadoSw);
					estadoSw++;
				} else {
					estadoSw = 0;
				}
				currentState = FSM_IDLE;
				break;

			case FSM_ENCODER_UP:
				encoderEventFlag = 0;  // Reiniciamos la bandera del evento del encoder
				if (estado == 4095) {  // Verificamos si hemos alcanzado el valor máximo
					estado = 0;       // Si es así, reiniciamos el contador a 0
				} else {
					estado++;          // En caso contrario, incrementamos el contador
				}
				currentState = FSM_IDLE;  // Volvemos al estado de reposo
				break;

			case FSM_ENCODER_DOWN:
				encoderEventFlag = 0;  // Reiniciamos la bandera del evento del encoder
				if (estado == 0) {     // Verificamos si hemos alcanzado el valor mínimo
					estado = 4095;     // Si es así, volvemos al valor máximo
				} else {
					estado--;          // En caso contrario, decrementamos el contador
				}
				currentState = FSM_IDLE;  // Volvemos al estado de reposo
				break;

			case FSM_UPDATE_DISPLAY:
				switch (turno) {
					case unidades:
						gpio_WritePin(&userDis1, SET);
						displayNumber(estado % 10);
						break;
					case decenas:
						gpio_WritePin(&userDis2, SET);
						displayNumber((estado / 10) % 10);
						break;
					case centenas:
						gpio_WritePin(&userDis3, SET);
						displayNumber((estado / 100) % 10);
						break;
					case miles:
						gpio_WritePin(&userDis4, SET);
						displayNumber((estado / 1000) % 10);
						break;
					default:
						turno = 0;
						break;
				}
				delay_ms(ms_delay);
				currentState = FSM_IDLE;
				break;

			default:
				currentState = FSM_IDLE;
				break;
		}

		}


}

//void togglePin(void){
	//	gpio_TogglePin(&userLed);  // Alternar el estado del LED
	//	delay_ms(400);
	//}
void displayNumber(uint8_t estado) {
    // Apagar todos los segmentos inicialmente
    // Encender los segmentos necesarios según el número
    if (estado == 0) {
        gpio_WritePin(&userSegA, SET);
        gpio_WritePin(&userSegB, SET);
        gpio_WritePin(&userSegC, SET);
        gpio_WritePin(&userSegD, SET);
        gpio_WritePin(&userSegE, SET);
        gpio_WritePin(&userSegF, SET);
    } else if (estado == 1) {
        gpio_WritePin(&userSegB, SET);
        gpio_WritePin(&userSegC, SET);
    } else if (estado == 2) {
        gpio_WritePin(&userSegA, SET);
        gpio_WritePin(&userSegB, SET);
        gpio_WritePin(&userSegD, SET);
        gpio_WritePin(&userSegE, SET);
        gpio_WritePin(&userSegG, SET);
    } else if (estado == 3) {
        gpio_WritePin(&userSegA, SET);
        gpio_WritePin(&userSegB, SET);
        gpio_WritePin(&userSegC, SET);
        gpio_WritePin(&userSegD, SET);
        gpio_WritePin(&userSegG, SET);
    } else if (estado == 4) {
        gpio_WritePin(&userSegB, SET);
        gpio_WritePin(&userSegC, SET);
        gpio_WritePin(&userSegF, SET);
        gpio_WritePin(&userSegG, SET);
    } else if (estado == 5) {
        gpio_WritePin(&userSegA, SET);
        gpio_WritePin(&userSegC, SET);
        gpio_WritePin(&userSegD, SET);
        gpio_WritePin(&userSegF, SET);
        gpio_WritePin(&userSegG, SET);
    } else if (estado == 6) {
        gpio_WritePin(&userSegA, SET);
        gpio_WritePin(&userSegC, SET);
        gpio_WritePin(&userSegD, SET);
        gpio_WritePin(&userSegE, SET);
        gpio_WritePin(&userSegF, SET);
        gpio_WritePin(&userSegG, SET);
    } else if (estado == 7) {
        gpio_WritePin(&userSegA, SET);
        gpio_WritePin(&userSegB, SET);
        gpio_WritePin(&userSegC, SET);
    } else if (estado == 8) {
        gpio_WritePin(&userSegA, SET);
        gpio_WritePin(&userSegB, SET);
        gpio_WritePin(&userSegC, SET);
        gpio_WritePin(&userSegD, SET);
        gpio_WritePin(&userSegE, SET);
        gpio_WritePin(&userSegF, SET);
        gpio_WritePin(&userSegG, SET);
    } else if (estado == 9) {
        gpio_WritePin(&userSegA, SET);
        gpio_WritePin(&userSegB, SET);
        gpio_WritePin(&userSegC, SET);
        gpio_WritePin(&userSegF, SET);
        gpio_WritePin(&userSegG, SET);
    }
}
void set_LED_Color(uint8_t color) {
    // Apagar todos los colores inicialmente
    gpio_WritePin(&ledRed, RESET);
    gpio_WritePin(&ledGreen, RESET);
    gpio_WritePin(&ledBlue, RESET);

    // Activar el color especificado
    switch (color) {
        case LED_OFF:
            gpio_WritePin(&ledRed, RESET);
            gpio_WritePin(&ledGreen, RESET);
            gpio_WritePin(&ledBlue, RESET);
            break;
        case LED_RED:
            gpio_WritePin(&ledRed, SET);
            break;
        case LED_GREEN:
            gpio_WritePin(&ledGreen, SET);
            break;
        case LED_BLUE:
            gpio_WritePin(&ledBlue, SET);

            break;
        case LED_CYAN:
            gpio_WritePin(&ledGreen, SET);
            gpio_WritePin(&ledBlue, SET);

            break;
        case LED_MAGENTA:
            gpio_WritePin(&ledRed, SET);
            gpio_WritePin(&ledBlue, SET);

            break;
        case LED_YELLOW:
            gpio_WritePin(&ledRed, SET);
            gpio_WritePin(&ledGreen, SET);

            break;
        case LED_WHITE:
            gpio_WritePin(&ledRed, SET);
            gpio_WritePin(&ledGreen, SET);
            gpio_WritePin(&ledBlue, SET);

            break;
    }
}
void init_System(void){

	//Configuramos el pin
	userLed.pGPIOx							= GPIOH;
	userLed.pinConfig.GPIO_PinNumber		= PIN_1;
	userLed.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userLed);

	//Configuramos el pin
	userDis1.pGPIOx							= GPIOA;
	userDis1.pinConfig.GPIO_PinNumber		= PIN_0;
	userDis1.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userDis1.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userDis1.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userDis1.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userDis1);

	//Configuramos el pin
	userDis2.pGPIOx							= GPIOA;
	userDis2.pinConfig.GPIO_PinNumber		= PIN_1;
	userDis2.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userDis2.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userDis2.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userDis2.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userDis2);

	//Configuramos el pin
	userDis3.pGPIOx							= GPIOB;
	userDis3.pinConfig.GPIO_PinNumber		= PIN_0;
	userDis3.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userDis3.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userDis3.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userDis3.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userDis3);


	//Configuramos el pin
	userDis4.pGPIOx							= GPIOA;
	userDis4.pinConfig.GPIO_PinNumber		= PIN_4;
	userDis4.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userDis4.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userDis4.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userDis4.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userDis4);
	//Configuramos el pin
	userSegA.pGPIOx							= GPIOA;
	userSegA.pinConfig.GPIO_PinNumber		= PIN_10;
	userSegA.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userSegA.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userSegA.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userSegA.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userSegA);
	//Configuramos el pin
	userSegB.pGPIOx							= GPIOB;
	userSegB.pinConfig.GPIO_PinNumber		= PIN_10;
	userSegB.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userSegB.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userSegB.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userSegB.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userSegB);
	//Configuramos el pin
	userSegC.pGPIOx							= GPIOB;
	userSegC.pinConfig.GPIO_PinNumber		= PIN_13;
	userSegC.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userSegC.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userSegC.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userSegC.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userSegC);
	//Configuramos el pin
	userSegD.pGPIOx							= GPIOC;
	userSegD.pinConfig.GPIO_PinNumber		= PIN_0;
	userSegD.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userSegD.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userSegD.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userSegD.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userSegD);
	//Configuramos el pin
	userSegE.pGPIOx							= GPIOC;
	userSegE.pinConfig.GPIO_PinNumber		= PIN_1;
	userSegE.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userSegE.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userSegE.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userSegE.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userSegE);
	//Configuramos el pin
	userSegF.pGPIOx							= GPIOB;
	userSegF.pinConfig.GPIO_PinNumber		= PIN_1;
	userSegF.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userSegF.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userSegF.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userSegF.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userSegF);

	//Configuramos el pin
	userSegG.pGPIOx							= GPIOB;
	userSegG.pinConfig.GPIO_PinNumber		= PIN_5;
	userSegG.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userSegG.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userSegG.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEED_MEDIUM;
	userSegG.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userSegG);
	//Configuramos el pin

	// Configurar el pin B12 (Rojo)
	ledRed.pGPIOx                           = GPIOC;
	ledRed.pinConfig.GPIO_PinNumber         = PIN_10;
	ledRed.pinConfig.GPIO_PinMode           = GPIO_MODE_OUT;
	ledRed.pinConfig.GPIO_PinOutputType     = GPIO_OTYPE_PUSHPULL;
	ledRed.pinConfig.GPIO_PinOutputSpeed    = GPIO_OSPEED_MEDIUM;
	ledRed.pinConfig.GPIO_PinPuPdControl    = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledRed);

	// Configurar el pin A11 (Verde)
	ledBlue.pGPIOx                         = GPIOC;
	ledBlue.pinConfig.GPIO_PinNumber       = PIN_12;
	ledBlue.pinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	ledBlue.pinConfig.GPIO_PinOutputType   = GPIO_OTYPE_PUSHPULL;
	ledBlue.pinConfig.GPIO_PinOutputSpeed  = GPIO_OSPEED_MEDIUM;
	ledBlue.pinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledBlue);

	// Configurar el pin C5 (Azul)
	ledGreen.pGPIOx                          = GPIOC;
	ledGreen.pinConfig.GPIO_PinNumber        = PIN_11;
	ledGreen.pinConfig.GPIO_PinMode          = GPIO_MODE_OUT;
	ledGreen.pinConfig.GPIO_PinOutputType    = GPIO_OTYPE_PUSHPULL;
	ledGreen.pinConfig.GPIO_PinOutputSpeed   = GPIO_OSPEED_MEDIUM;
	ledGreen.pinConfig.GPIO_PinPuPdControl   = GPIO_PUPDR_NOTHING;
	gpio_Config(&ledGreen);

	userClk.pGPIOx							= GPIOA;
	userClk.pinConfig.GPIO_PinNumber		= PIN_12;
	userClk.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	userClk.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan el puerto.
	gpio_Config(&userClk);

	//Configuramos el pin
	userData.pGPIOx							= GPIOC;
	userData.pinConfig.GPIO_PinNumber		= PIN_6;
	userData.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	userData.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan 	el puerto.
	gpio_Config(&userData);

	//Configuramos el pin
	userSw.pGPIOx							= GPIOC;
	userSw.pinConfig.GPIO_PinNumber		    = PIN_9;
	userSw.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	userSw.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Cargamos la configuracion  en los registros que gobiernan 	el puerto.
	gpio_Config(&userSw);

	config_SysTick_ms(0);

	blinkyTimer.pTIMx								= TIM2;
	blinkyTimer.TIMx_Config.TIMx_mode				= TIMER_UP_COUNTER;
	blinkyTimer.TIMx_Config.TIMx_Prescaler  		= 16000; //1ms conversion
	blinkyTimer.TIMx_Config.TIMx_Period				= 500;
	blinkyTimer.TIMx_Config.TIMx_InterruptEnable 	= TIMER_INT_ENABLE;

	timer_Config(&blinkyTimer);
	timer_SetState(&blinkyTimer, TIMER_ON);

	displayTimer.pTIMx								= TIM3;
	displayTimer.TIMx_Config.TIMx_mode				= TIMER_UP_COUNTER;
	displayTimer.TIMx_Config.TIMx_Prescaler  		= 16000; //1ms conversion
	displayTimer.TIMx_Config.TIMx_Period			= ms_delay;
	displayTimer.TIMx_Config.TIMx_InterruptEnable 	= TIMER_INT_ENABLE;

	timer_Config(&displayTimer);
	timer_SetState(&displayTimer, TIMER_ON);

	extiClk.pGPIOHandler					= &userClk;
	extiClk.edgeType						= EXTERNAL_INTERRUPT_RISING_EDGE;
	exti_Config(&extiClk);

	extiSw.pGPIOHandler					= &userSw;
	extiSw.edgeType						= EXTERNAL_INTERRUPT_RISING_EDGE;
	exti_Config(&extiSw);

	gpio_WritePin(&userLed, SET);
}

/*
 * Overwrite function
 */
void Timer2_Callback(void){
	gpio_TogglePin(&userLed);
}
void Timer3_Callback(void){
	turno++;
	currentState = FSM_UPDATE_DISPLAY;
}
void callback_ExtInt12(void) {
    data = gpio_ReadPin(&userData);
    clock = gpio_ReadPin(&userClk);  // Detecta el cambio en el encoder
    encoderEventFlag = 1;            // Marca evento del encoder

    // Detección de flanco de bajada en el clock
    if (prevClock == 1 && clock == 0) {
        if (data == 1) {
            currentState = FSM_ENCODER_UP;   // Incrementar si data está en alto
        } else {
            currentState = FSM_ENCODER_DOWN; // Decrementar si data está en bajo
        }
    }
    prevClock = clock;  // Actualizar el estado anterior del clock
}
void callback_ExtInt9(void){
	uint32_t currentTime = getTicks_ms();  // Obtener el tiempo actual en milisegundos

	// Verificar si ha pasado el tiempo de anti-rebote
	if (!debounceInProgress && (currentTime - lastDebounceTime) > DEBOUNCE_DELAY_MS) {
		sw = gpio_ReadPin(&userSw);   // Leer el estado del switch
		swEventFlag = 1;              // Marca evento del switch
		currentState = FSM_CHANGE_LED_COLOR;  // Cambiar el estado del FSM

		lastDebounceTime = currentTime;  // Actualizar el tiempo del último evento
		debounceInProgress = 1;          // Indicar que el anti-rebote está en proceso
	}
}

/*
 * Esta función sirve para detectar problemas de parametros
 * incorrectos al momento de ejecutar un programa.
 */
void assert_failed(uint8_t* file, uint32_t line){
	while(1){
		//Problems
	}
}
