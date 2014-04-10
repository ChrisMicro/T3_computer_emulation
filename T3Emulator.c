/*
 ============================================================================
 Name        : T3Emulator.c
 Author      : chris
 Version     :
 Copyright   : GPL V3
 Description : T3 emulator in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "t3.h"
#include "systemout.h"

uint16_t testProg[]=
{
    PLUS, 1,
    JMPU, 0,
    LDA, 0321,
    PLPC,
    GSBU, 2,
    SAF,
    SAF,
    LDA, 0000,     //  0123->A
    LDA+ABS, 0002, //  memory[0100]->A
    LDA+IND, 0003, //  memory[memory[0100]]->A
    LDA, 04321,
    SAF,
    JMPR+AF_FLAG, 2,
    CAF,
    JMPR+AF_FLAG, 2,
};


int main(void) {
  //printf("%04o:\n\r",testProg[2]);
    Cpu_t cpu;
	SYSTEMOUT("T3Emulator"); /* prints T3Emulator */
	simulatorReset(&cpu);
	memcpy(&cpu.M[0],testProg,sizeof(testProg));

    SYSTEMOUT("run"); /* prints T3Emulator */
	do{
	  showCpu(&cpu);
	  executeVm(&cpu);
	}while(getchar()!='x');

	SYSTEMOUT("good bye");
	return EXIT_SUCCESS;
}
