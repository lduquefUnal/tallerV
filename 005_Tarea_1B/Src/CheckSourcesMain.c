/*re
 * CheckSourcesMain.c
 *
 *  Created on: Dec 2, 2024
 *      Author: namontoy
 */

#include <stdint.h>
#include <stdio.h>
#include <stm32f4xx.h>

/* Definiciond de variables */
uint32_t rotation;
int16_t temperatureValue;
int dummy;

// Definicion de las cabeceras de las funciones del main
extern void configMagic(void);
int16_t getTemperature(uint8_t getData);
uint32_t checkRotation(void);
uint8_t leaking(uint16_t const range);
uint8_t positiveFunction(uint8_t data);
int dostuff(char *data, int value);
uint8_t weirdFunction(uint8_t data);
// defino variables para visualizar los resultados en express
uint32_t result1;
int16_t result2;
uint8_t result3;
uint8_t result4;
uint8_t result5;

int result6;
/*
 * The main function, where everything happens.
 */
int main(void)
{
	/* Configuramos la magia! */
	configMagic();

	// Ciclo principal
	printf("Hola Mundo!\n");
	rotation =27 ;
	/* Probando diferentes funciones */
	result1=checkRotation();
	result2 = getTemperature(1);
	result3 = leaking(7);
	result4 = positiveFunction(2);
	result6 = dostuff("abc",1024);
	result5 = weirdFunction(26);

	while (1) {
	}

	return 0;
}

/* checkRotation devuelve un 1 o 0 , pero las condiciones(if) están mal
 * implementadas  no existe un numero>45 y que luego sea numero<23
 * por lo que el return será siempre de 1; como 45 siempre es mayor que 25 ,
 * no es necesario la 2 verificacion con el if(rotation>45)*eliminar linea*
 *
 * almacenar el valor en un result_!
 * */

uint32_t checkRotation(void){

//	if(rotation > 25){
//		if(rotation > 45){
//			if(rotation < 23){
//				return 0;
//			}
//		}
//		return 1;
//	}
//	return 1;

	if (rotation>25){
	return 1;
	}else if (rotation<23){
	return 0;
	}return 1 ;
}

/*i--  hace que el for sea infinito   ya que siempre i<10
 * temperaturaValue se redefine en cada ciclo , pero el unico valor
 * importante es cuando i = 9  ,y es 9*35= 315 lo que retorna
 *  */
int16_t getTemperature(uint8_t getData){
	if(getData == 1){
		for(int i = 0; i < 10; i++){
			temperatureValue = i*35;
//			i--;
		}
		return temperatureValue;
	}
	return 0;
}

/* range debe ser < 10 , ya que si es mayor no se puede definir
 * el char a[10]se sobre escribe y sale error, se soluciona aumentando el tamaño de a
 * a[range] para evitar esto , y se debe retomar el valor
 * a[range-1] ya que se cuenta desde el 0*/
uint8_t leaking(uint16_t const range){
//	char a[10];
//	/* Utilice una linea de codigo del ciclo FOR y luego la otra, ¿que observa en la salida del cppcheck?*/
//	for(uint8_t size = 0; size < range; size++){
//	//for(uint8_t size = 0; size < 350; size++){
//		a[size] = size*2;
//	}
//	/* Utilice una linea de codigo del ciclo FOR y luego la otra, ¿que observa en la salida del cppcheck?*/
//	//return a[9];
//	return a[10];
	char a[255]={0};
	/* Utilice una linea de codigo del ciclo FOR y luego la otra, ¿que observa en la salida del cppcheck?*/

	for(uint8_t size = 0; size < range; size++){
		//for(uint8_t size = 0; size < 350; size++){
			a[size] = size*2;
		}
		/* Utilice una linea de codigo del ciclo FOR y luego la otra, ¿que observa en la salida del cppcheck?*/
		//return a[9];
		return a[range-1];
}

/* la comparación data== 1 no permite saber si el  numero es positivo
 * por lo que se debe usar data>0 */
uint8_t positiveFunction(uint8_t data){
//	if(data == 1){
//		return 1;
//	}else{
//		return 1;
//	}
	if(data > 0){
			return 1;
		}else{
			return 0;
		}
}

/* la variable weird no se usa de manera adecuada
 * elimiar el weird y arreglar el if para que la condición/verificación
 * data == 25 tenga sentido
 *
 *  */
uint8_t weirdFunction(uint8_t data){
//	uint8_t weird = 10;
//	if((weird = data) == 25){
//		return 0;
//	}else{
//		return 0;
//	}
	if(data == 25){
		return 0;
	}else{
		return 0;
	}
}

/* data[125] está fuera del los limites definidos para data
 * pudiendo causar desbordamiento de memoría
 *definit data[value]
 * */
int dostuff(char *data, int value){
//	data[125] = 200;
//	dummy = value + 1;
//
//	return 0;
	if (value < 5) {
	        data[value] = 200; // Acceso seguro
	    } else {
	        // Manejo de error: arreglo demasiado pequeño
	        return 0;
	    }

	    dummy = value + 1;

	    return 1;
}
