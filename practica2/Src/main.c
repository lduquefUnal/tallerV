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

// Definicion de las cabeceras de las funciones del main
uint8_t Mascara1=0;
uint16_t Mascara2=0;
uint32_t Mascara3=0;

uint8_t Mascara4;
uint16_t Mascara5;
uint32_t Mascara6;
uint8_t Mascara7=0;
uint16_t Mascara8=0;
uint32_t Mascara9=0;
/*
 * The main function, where everything happens.
 */
int main(void)
{
// este es un comentario para poner a prueba git hub
	Mascara1 &= 0b1 << 1 ;
	Mascara2 |= 0xFA;
	Mascara3 = (Mascara1|Mascara2)|(1<<31);
//	 << shif en posición n ,  1<< n
	Mascara4 = (Mascara3)|(Mascara2<<14) ;
	Mascara5 = (Mascara3 & Mascara4);
	Mascara6 = 0b1 << 31;
	Mascara7 = (Mascara5 & Mascara6)>>30;
//	Mascara9 = Mascara6;
	while (1) {
	}

	return 0;
}

