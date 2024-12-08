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

#include "main.h"
fsm_states_t  fsm_torniquete = {0} ;
char bufferData[16];

void initProgram(void);
e_PosibleStates state_machine_action(uint8_t event);


// Definicion de las cabeceras de las funciones del main

/*
 * The main function, where everything happens.
 */
int main(void)
{
	/* Configuramos la magia! */
	configMagic();
	initProgram();
	printf("hola mundo jajaj");
	// Ciclo principal

	while (1) {
		read_input(bufferData);

		if(bufferData[0]=='c'){
			printf("Coin inserted \n");
			state_machine_action(bufferData[0]);
		}
		else if(bufferData[0] == 'p' ){
			printf("Push entrance \n");
			state_machine_action(bufferData[0]);
		}
		else {
			printf("Wrong Key... \n");
		}
	}

	return 0;
}

void initProgram(void){
	fsm_torniquete.state= LOCKED;
}

e_PosibleStates state_machine_action(uint8_t event){
	switch (fsm_torniquete.state) {
	case LOCKED:
	{
		switch (event)
		{
		case 'c':
		{
			printf("it was LOCKED -> now it is UNLOCKED \n");
			printf("state: UNLOCKED \n");
			fsm_torniquete.state= UNLOCKED;
			break;
		}
		case 'p':
		{
			printf("you cannot cross, please insert coin¡ \n");
			printf("state: LOCKED \n");
			fsm_torniquete.state = LOCKED;
			break;
		}
		return fsm_torniquete.state;

	}
	case UNLOCKED:
		{
			switch (event)
			{
			case 'c':
			{
				printf("it is already UNLOCKED -> now it is UNLOCKED \n");
				printf("state: UNLOCKED \n");
				fsm_torniquete.state= UNLOCKED;
				break;
			}
			case 'p':
			{
				printf("you crossed, now is LOCKED¡ \n");
				printf("state: LOCKED \n");
				fsm_torniquete.accessCounter++;
				fsm_torniquete.state = LOCKED;
				break;
			}
			return fsm_torniquete.state;
			}
		}
	}
}};
