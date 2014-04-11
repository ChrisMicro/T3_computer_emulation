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
  cpu->Pc=1;
  cpu->Sp=0;
  cpu->A=0;
  cpu->flags=(1<<ONE1_FLAG)|(1<<ONE_FLAG);

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
      DISASM("NOP2");
      INCPC(cpu);
    }break;
    //#define MINUS  00006 // A-M-1 -> A

    //#define PLUS   00011 // A+M -> A
    case PLUS:{
      DISASM("PLUS");
      INCPC(cpu);
      tmp=readMemory(cpu,mode);
      cpu->A+=tmp;
      if(cpu->flags&(1<<AF_FLAG))cpu->A++;
      if(cpu->A>0777)cpu->flags|=(1<<AF_FLAG);
      else cpu->flags&=~(1<<AF_FLAG);
      cpu->A&=0777;
    }break;
    //******************************************************
    // stack instructions
    //******************************************************
    //#define PLPC   00600 // pull PC ( return )
    case PLPC:{
      DISASM("PLPC (return)");
      cpu->Pc=pop(cpu)-1;
    }break;
    //#define PLP    00601 // pull P
    case PLP:{
      DISASM("PLP ( display )");
      cpu->display=pop(cpu)-1;
    }break;
    //#define PLIO   00602 // pull I/O
    case PLIO:{
      DISASM("PLIO (outport)");
      cpu->outport=pop(cpu)-1;
    }break;
    //#define PLA    00603 // pull A
    case PLA:{
      DISASM("PLA");
      cpu->A=pop(cpu)-1;
    }break;
    //#define PHPC   00604 // push PC
    case PHPC:{
      DISASM("PHPC");
      push(cpu,cpu->Pc); // warning: check if original T3 pushes next address
    }break;
    //#define PHP    00614 // push P
    case PHP:{
      DISASM("PHP (keys)");
      push(cpu,cpu->keys);
    }break;
    //#define PHIO   00624 // push I/O
    case PHIO:{
      DISASM("PHIO (outport)");
      push(cpu,cpu->outport);
    }break;
    //#define PHA    00634 // push A
    case PHA:{
      DISASM("PHA");
      push(cpu,cpu->A);
    }break;
    //******************************************************
    // register memory transfer instructions
    //******************************************************
    //#define STPC   00100 // PC -> M
    case STPC:{
      DISASM("STPC");
      INCPC(cpu);
      writeMemory(cpu,mode,cpu->Pc);
    }break;
    //#define STP    00110 // P -> M
    case STP:{
      DISASM("STP");
      INCPC(cpu);
      writeMemory(cpu,mode,cpu->keys);
    }break;
    //#define STIO   00120 // I/O -> M
    case STIO:{
      DISASM("STIO");
      INCPC(cpu);
      writeMemory(cpu,mode,cpu->inport);
    }break;
    //#define STA    00130 // A -> M
    case STA:{
      DISASM("STA");
      INCPC(cpu);
      writeMemory(cpu,mode,cpu->A);
    }break;
    //#define LDPC   00104 // M -> PC
    case LDPC:{
      DISASM("LDA");
      INCPC(cpu);
      cpu->A=readMemory(cpu,mode);
    }break;
    //#define LDP    00105 // M -> P
    case LDP:{
      DISASM("LDP");
      INCPC(cpu);
      cpu->display=readMemory(cpu,mode);
    }break;
    //#define LDIO   00106 // M -> I/O
    case LDIO:{
      DISASM("LDP");
      INCPC(cpu);
      cpu->outport=readMemory(cpu,mode);
    }break;
    //#define LDA    00107 // M -> A
    case LDA:{
      DISASM("LDA");
      INCPC(cpu);
      cpu->A=readMemory(cpu,mode);
    }break;
    // compare instructions
    //******************************************************
    // compare instruction
    //******************************************************
    //#define CMP    00200 // A-M -> FLG
    case CMP:{
      DISASM("CMP");
      INCPC(cpu);
      tmp=readMemory(cpu,mode);
      //#define EQ_FLAG      0  // A = OPR
      if(tmp==cpu->A)cpu->flags|=(1<<EQ_FLAG);
      //#define GT_FLAG      1  // A > OPR
      if(tmp>cpu->A)cpu->flags|=(1<<GT_FLAG);
      //#define SM_FLAG      2  // A < OPR
      if(tmp<cpu->A)cpu->flags|=(1<<SM_FLAG);
      //#define ALU_FLAG     3  //
      //#define MSB_FLAG     4  // MSB of A
      if(cpu->A&0400)cpu->flags|=(1<<MSB_FLAG);
    }break;
    //******************************************************
    // machine control instructions
    //******************************************************
    //#define RST    00400 // 0001->PC
    case RST:{
      DISASM("RST");
      cpu->Pc=1;
    }break;
    //#define STOP   00401 // stop
    case STOP:{
      DISASM("STOP");
      cpu->Pc--;
    }break;
    //#define RSD    00402 // reset display
    case RSD:{
      DISASM("RSD");
      cpu->display=0;
    }break;
    //#define SHIB   00403 // select high bank
    case SHIB:{
      DISASM("select high bank");
      cpu->bank=1;
    }break;
    //#define SLOB   00404 // select low bank
    case SLOB:{
      DISASM("select low bank");
      cpu->bank=0;
    }break;
    //#define RSSP   00405 // 0->SP, reset stack pointer
    case RSSP:{
      DISASM("RSSP");
      cpu->Sp=0;
    }break;
    //#define RSA    00406 // 0->ACCU, reset accumulator
    case RSA:{
      DISASM("RSA");
      cpu->A=0;
    }break;
    //#define STB    00407 // strobe ?
    case STB:{
      DISASM("strobe?");
    }break;
    //SAF    00410 // set accu flag ( AF_FLAG )
    case SAF:{
      DISASM("SAF");
      cpu->flags|=(1<<AF_FLAG);
    }break;
    //#define CAF    00411 // clear accu flag ( AF_FLAG )
    case CAF:{
      DISASM("CAF");
      cpu->flags&=~(1<<AF_FLAG);
    }break;
    //#define SSF    00412 // set shift flag ( SF_FLAG )
    case SSF:{
      DISASM("SSF");
      cpu->flags|=(1<<SF_FLAG);
    }break;
    //#define CSF    00413 // clear shift flag ( SF_FLAG )
    case CSF:{
      DISASM("CSF");
      cpu->flags&=~(1<<SF_FLAG);
    }break;
    //#define NOP1   00414 // one word nop
    case NOP1:{
      DISASM("NOP1");
    }break;
    //******************************************************
    // register register transfer instructions
    //******************************************************
    //#define TPCP   00501 // PC -> P // PC -> display
    case TPCP:{
      DISASM("TPCP");
      cpu->display=cpu->Pc;
    }break;
    //#define TPCIO  00502 // PC -> IO ( outport )
    case TPCIO:{
      DISASM("TPCIO");
      cpu->outport=cpu->Pc;
    }break;
    //#define TPCA   00503 // PC -> A
    case TPCA:{
      DISASM("TPCA");
      cpu->A=cpu->Pc;
    }break;
    //#define TPPC   00510 // (keys) P -> PC
    case TPPC:{
      DISASM("TPPC (keys)");
      cpu->Pc=cpu->keys;
    }break;
    //#define TPP    00511 // P -> P read keyboard, write display
    case TPP:{
      DISASM("TPP (keys->display)");
      cpu->display=cpu->keys;
    }break;
    //#define TPIO   00512 // P -> IO
    case TPIO:{
      DISASM("TPIO (keys->outport)");
      cpu->outport=cpu->keys;
    }break;
    //#define TPA    00513 // P -> A // keys -> A
    case TPA:{
      DISASM("TPIO (keys->outport)");
      cpu->A=cpu->keys;
    }break;
    //#define TIOPC  00520 // IO->PC
    case TIOPC:{
      DISASM("TIOPC (inport->pc)");
      cpu->Pc=cpu->inport;
    }break;
    //#define TIOP   00521 // IO->P
    case TIOP:{
      DISASM("TIOP (inport->display)");
      cpu->display=cpu->inport;
    }break;
    //#define TIOA   00523 // IO->A
    case TIOA:{
      DISASM("TIOA");
      cpu->A=cpu->inport;
    }break;
    //#define TAPC   00530 // A->PC ( variable jump )
    case TAPC:{
      DISASM("TAPC (jmp(A))");
      cpu->Pc=cpu->A;
    }break;
    //#define TAP    00531 // A->P store A in display
    case TAP:{
      DISASM("TAP");
      cpu->display=cpu->A;
    }break;
    //#define TAIO   00532 // A->IO store A in outport
    case TAIO:{
      DISASM("TAIO");
      cpu->outport=cpu->A;
    }break;

    //******************************************************
    // masked functions
    //******************************************************
    default:{
      tmp=instr&JMPMASK;
      uint8_t flags;
      flags=instr&FLAGMASK;
      //ERROR("instr", tmp);
      switch(tmp)
      {
      //******************************************************
      // program flow instructions
      //******************************************************
        case JMPS:{
          DISASM("JMPS");
          INCPC(cpu);
          if(cpu->flags&(1<<flags))
            {
              DISASM("jump");
              cpu->Pc=readMemory(cpu,mode)-1;
            }
        }break;
        case JMPR:{
          DISASM("JMPR");
          INCPC(cpu);
          if(!(cpu->flags&(1<<flags)))
          {
            DISASM("jump");
            cpu->Pc=readMemory(cpu,mode)-1;
          }
        }break;
        case GSBS:{
          DISASM("GSBS");
          INCPC(cpu);
          if(cpu->flags&(1<<flags))
          {
            DISASM(" (call)");
            push(cpu,cpu->Pc+1);
            cpu->Pc=readMemory(cpu,mode)-1;
          }
        }break;
        case GSBR:{
          DISASM("GSBR");
          INCPC(cpu);
          if(!(cpu->flags&(1<<flags)))
          {
            DISASM(" (call)");
            push(cpu,cpu->Pc+1);
            cpu->Pc=readMemory(cpu,mode)-1;
          }
        }break;

        default:
        {
          //******************************************************
          // shift instructions
          //******************************************************
          tmp=instr&SHIFTMASK;
          uint8_t shift=instr&07;
          switch(tmp)
          {
          //#define ROL 00700 // rotate left n bits
          case ROL:{
            tmp=cpu->A;
            tmp=tmp<<shift;
            tmp|=cpu->A>>(13-shift);
            if(tmp&010000)cpu->flags|=(1<<SF_FLAG);
            else cpu->flags&=~(1<<SF_FLAG);
            cpu->A=tmp&0777;
          }
          //#define ROR 00710 // rotate right n bits
          case ROR:{
            tmp=cpu->A;
            tmp=tmp>>shift;
            tmp|=cpu->A<<(13-shift);
            if(tmp&010000)cpu->flags|=(1<<SF_FLAG);
            else cpu->flags&=~(1<<SF_FLAG);
            cpu->A=tmp&0777;
          }
          //#define SFL 00720 // shift left n bits
          case SFL:{
            tmp=cpu->A;
            tmp=tmp<<shift;
            cpu->A=tmp&0777;
            if(tmp&010000)cpu->flags|=(1<<SF_FLAG);
            else cpu->flags&=~(1<<SF_FLAG);
          }
          //#define SFR 00730 // shift right n bits
          case SFR:{
            tmp=cpu->A;
            tmp=tmp>>shift;
            cpu->A=tmp&0777;
            if((cpu->A)>>(shift-1)&1)cpu->flags|=(1<<SF_FLAG);
            else cpu->flags&=~(1<<SF_FLAG);
          }
            default:
            {
              ERROR("error: unknown instruction",instr);
              simulatorReset(cpu);
              cpu->Pc--;
            }break;
          }
        }
      }
    }break;
  }
  INCPC(cpu);
}
