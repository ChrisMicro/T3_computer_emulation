/*
 ============================================================================
 Name        : t3.c
 Author      : chris
 Version     :
 Copyright   : GPL V3
 Description : T3 virtual machine in C, Ansi-style
 ============================================================================
 */

#include <stdint.h>
#include <stdio.h>
#include "t3.h"
#include "systemout.h"

void simulatorReset(Cpu_t *cpu)
{
  cpu->bank=0;
  cpu->Pc=0; // warning: in original T3 Pc is set to 1
  cpu->Sp=0;
  cpu->A=01234;
  //cpu->flags=(1<<ONE1_FLAG)|(1<<ONE_FLAG);

}
void dump(Cpu_t *cpu,uint16_t num)
{
  uint16_t n;
  for(n=0;n<num;n++) ERROR("",cpu->M[cpu->bank][n]);
}
void dumpStack(Cpu_t *cpu,uint16_t num)
{
  uint16_t n;
  ERROR("stack:\n",0);
  for(n=0;n<num;n++) ERROR("",cpu->stack[n]);
}
uint16_t readMemory(Cpu_t *cpu, uint16_t mode)
{
  uint16_t tmp;

  switch(mode)
  {
    case IMM:{
      tmp=cpu->M[cpu->bank][cpu->Pc];
    }break;
    case ABS:{
      tmp=cpu->M[cpu->bank][cpu->Pc];
      tmp=cpu->M[cpu->bank][tmp];
    }break;
    case IND:{
      tmp=cpu->M[cpu->bank][cpu->Pc];
      tmp=cpu->M[cpu->bank][tmp];
      tmp=cpu->M[cpu->bank][tmp];
    }break;
    default:
    {
      ERROR("mem read mode error ",mode);
      simulatorReset(cpu);
      tmp=0;
    }break;
  }
  return tmp;
}
void writeMemory(Cpu_t *cpu, uint16_t mode,uint16_t value)
{
  uint16_t dest;

  switch(mode)
  {
    case IMM:{
      dest=cpu->Pc;
      if(dest>M_SIZE)
      {
        ERROR("error: memwrite error",dest);
      }
      else
      {
        cpu->M[cpu->bank][dest]=cpu->A;
      }
    }break;
    case ABS:{

      dest=cpu->Pc;
      if(dest>M_SIZE)
      {
        ERROR("error: memwrite error",dest);
      }
      else dest=cpu->M[cpu->bank][dest];
      if(dest>M_SIZE)
      {
        ERROR("error: memwrite error",dest);
      }
      else
      {
        cpu->M[cpu->bank][dest]=cpu->A;
      }
    }break;
    case IND:{
      dest=cpu->Pc;
      if(dest>M_SIZE)
      {
        ERROR("error: memwrite error",dest);
      }
      else dest=cpu->M[cpu->bank][dest];
      if(dest>M_SIZE)
      {
        ERROR("error: memwrite error",dest);
      }
      else dest=cpu->M[cpu->bank][dest];
      if(dest>M_SIZE)
      {
        ERROR("error: memwrite error",dest);
      }
      else
      {
        cpu->M[cpu->bank][dest]=cpu->A;
      }
    }break;
    default:
    {
      ERROR("mem write mode error ",mode);
      simulatorReset(cpu);

    }break;
  }
}
void showCpu(Cpu_t *cpu)
{
    printf("%04o:",cpu->Pc);
    printf("%04o ",readMemory(cpu,0));
    printf("sp:%04o ",cpu->Sp);
    printf("A:%04o ",cpu->A);
    printf("SF:%01o ",((cpu->flags)>>SF_FLAG)&1);
    printf("AF:%01o ",((cpu->flags)>>AF_FLAG)&1);

    /*
    uint16_t M[2][M_SIZE]; // memory
    uint16_t stack[STACK_SIZE];
    uint16_t ACCU;
    uint16_t Sp;
    uint16_t Pc; // program counter
    uint16_t flags;
    uint16_t display; // 4 digit nixie tube display ( panel output P )
    uint16_t keys;    // 12 input keys ( panel input P )
    uint16_t inport;  // 12 bit user input lines ( IO )
    uint16_t outport; // 12 bit user output lines ( IO )
    */
    dump(cpu,10);
}
void push(Cpu_t *cpu,uint16_t value)
{
  dumpStack(cpu,3);
    if((cpu->Sp)<STACK_SIZE) cpu->stack[cpu->Sp++]=value; // push constant
    else
    {
        ERROR("data stack overflow",cpu->Sp);
        ERROR("reset vm",0);
        simulatorReset(cpu);
    }
}
uint16_t pop(Cpu_t *cpu)
{
    uint16_t value=0;
    if((cpu->Sp)>0) value=cpu->stack[--(cpu->Sp)];
    else
    {
        ERROR("data stack empty",cpu->Sp);
        ERROR("reset vm",0);
        simulatorReset(cpu);
    }
    return value;
}
void incrementPc(Cpu_t *cpu)
{
  cpu->Pc++;
  if(cpu->Pc>M_SIZE){
    ERROR("Pc overflow",cpu->Pc);
    simulatorReset(cpu);
  }
}

#define INCPC(cpu) {incrementPc(cpu);}

void executeVm(Cpu_t *cpu)
{
  uint16_t tmp;
  uint16_t mode;
  uint16_t instr;
  tmp=readMemory(cpu,IMM);
  mode=tmp&03000;
  instr=tmp&0777;

  switch(instr)
  {
  // ALU instructions
  // two word instructions
  // instruction with addressing mode, operand
  //#define NOP2   00000 // A->A nop with 2 instructions
  case NOP2:{
    INCPC(cpu);

  }break;
  //#define MINUS  00006 // A-M-1 -> A

  //#define PLUS   00011 // A+M -> A
  case PLUS:{
    INCPC(cpu);
    tmp=readMemory(cpu,mode);
    cpu->A+=tmp;
    if(cpu->flags&(1<<AF_FLAG))cpu->A++;
    if(cpu->A>0777)cpu->flags|=(1<<AF_FLAG);
    else cpu->flags&=~(1<<AF_FLAG);
    cpu->A&=0777;
  }break;
    case LDA:{
      INCPC(cpu);
      cpu->A=readMemory(cpu,mode);
    }break;
    case STA:{
      INCPC(cpu);
      writeMemory(cpu,mode,cpu->A);
    }break;
    //SAF    00410 // set accu flag ( AF_FLAG )
    case SAF:{
      cpu->flags|=(1<<AF_FLAG);
    }break;
    case CAF:{
      cpu->flags&=~(1<<AF_FLAG);
    }break;
    // stack operations
    // pull pc = return
    case PLPC:{
      cpu->Pc=pop(cpu)-1;
    }break;

    default:{
      tmp=instr&JMPMASK;
      uint8_t flags;
      flags=instr&FLAGMASK;
      //ERROR("instr", tmp);
      switch(tmp)
      {
        case JMPS:{
          INCPC(cpu);
          if(cpu->flags&(1<<flags)) cpu->Pc=readMemory(cpu,mode)-1;
        }break;
        case JMPR:{
          INCPC(cpu);
          if(!(cpu->flags&(1<<flags))) cpu->Pc=readMemory(cpu,mode)-1;
        }break;
        case GSBS:{
          INCPC(cpu);
          if(cpu->flags&(1<<flags))
          {
            push(cpu,cpu->Pc+1);
            cpu->Pc=readMemory(cpu,mode)-1;
          }
        }break;
        case GSBR:{
          INCPC(cpu);
          if(!(cpu->flags&(1<<flags)))
          {
            push(cpu,cpu->Pc+1);
            cpu->Pc=readMemory(cpu,mode)-1;
          }
        }break;
        default:
        {
          ERROR("error: unknown instruction",instr);
          simulatorReset(cpu);
          cpu->Pc--;
        }
      }
    }
  }
  INCPC(cpu);
}
