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
extern void configMagic(void);

/*
 * The main function, where everything happens.
 */
int main(void)
{
	/* Configuramos la magia! */
	configMagic();

	// Ciclo principal


		printf("Hola Mundo!\n");
		printf("Characters: %c %c\n", 'a', 65);
		printf("Decimals: %d %ld\n", 1977, 650000L);
		printf("Preceding with blanks: %10d\n", 1977);
		printf("Preceding with zeros: %010d\n", 1977);
		printf("Some different radices: %d %x %o %#x %#o\n", 100, 100, 100, 100,
				100);
		printf("floats: %4.2f %+.0e %E\n", 3.1416, 3.1416, 3.1416);
		printf("Width trick: %*d\n", 5, 10);
		printf("%s\n", "A string");

	while (1) {
	}

	return 0;
}

