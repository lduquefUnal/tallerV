#include <stdint.h>
#include <stdio.h>
#include <stm32f4xx.h>



#ifndef MAIN_H_
#define MAIN_H_


typedef enum
{
	LOCKED,
	UNLOCKED
}e_PosibleStates;

typedef struct
{
	uint32_t accessCounter ;
	e_PosibleStates state;
}fsm_states_t;


extern void configMagic(void);
extern void clear_scanf(void);
extern void	read_input(char * data);

#endif
