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
#include "example.h"

uint16_t testProg[]=
{
    NOP1, // this instruction is not executed, reset vector set to next address (1)
    // programm start at address 1
    LDA, 0,
    OR, 7,
    AND, 2,
    EXOR, 7,

    JMPU, 1,
    LDA, 07,
    AND, 02,
    PLPC,
    GSBU, 3,
    SAF,
    SAF,
    LDA, 0000,     //  0->A
    LDA+ABS, 0002, //  memory[2]->A
    LDA+IND, 0003, //  memory[memory[2]]->A
    LDA, 04321,
    SAF,
    JMPR+AF_FLAG, 2,
    CAF,
    JMPR+AF_FLAG, 1,
};

void dump(Cpu_t *cpu,uint16_t num)
{
  uint16_t n;
  SYSTEMOUT("memory:");
  for(n=0;n<num;n++) SYSTEMOUTOCT("",cpu->M[cpu->bank][n]);
  SYSTEMOUTCR;
}
void dumpStack(Cpu_t *cpu,uint16_t num)
{
  uint16_t n;
  SYSTEMOUT("stack:");
  for(n=0;n<num;n++) SYSTEMOUTOCT("",cpu->stack[n]);
  SYSTEMOUTCR;
}
int main(void)
{
    uint8_t c;

    Cpu_t cpu;
	SYSTEMOUT("T3Emulator"); /* prints T3Emulator */
	simulatorReset(&cpu);
	loadProgram(&cpu.M[0][0]);
	//memcpy(&cpu.M[0],testProg,sizeof(testProg));
	SYSTEMOUTCR;

    SYSTEMOUT("commands"); /* prints T3Emulator */
    SYSTEMOUT("d: dump"); /* prints T3Emulator */
    SYSTEMOUT("x: exit"); /* prints T3Emulator */
    SYSTEMOUT("default: single step"); /* prints T3Emulator */
    SYSTEMOUTCR;

	do{
	  c=getchar();
	  switch(c)
	  {
        case 'd':
        {
          dumpStack(&cpu,8);
          dump(&cpu,16);
          SYSTEMOUTCR;
        }break;
        default:
        {
          showCpu(&cpu);
          executeVm(&cpu);
        }break;
	  }
	}while(c!='x');

	SYSTEMOUT("good bye");
	return EXIT_SUCCESS;
}
