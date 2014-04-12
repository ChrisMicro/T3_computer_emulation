#include <stdint.h>
#include "example.h"
#include "systemout.h"

#define LABEL 0x8000 // use the highest bit for label indication
#define DEST  0x4000 // use the next lower bit for destination

#define MAXNUMBEROFLABELS 100 // number of labels
uint16_t Labels[MAXNUMBEROFLABELS];

uint16_t assemblerProgram[]=
{
          NOP1, // this instruction is not executed, reset vector set to next address (1)
                // program start at address 1

          JMPU, DEST+1, // jump unconditional to program start

//****** reserve some memory space  ************
LABEL+20,
          NOP1,         // data stack pointer
LABEL+21,
          NOP1,         // data stack first entry
          NOP1,
          NOP1,
          NOP1,
          NOP1,

//***** end of reserved space ******************

          // program start
LABEL+1,
          // initialize data stack pointer
          LDA, DEST+21, // data stack entries ->A
          STA+ABS, DEST+20, // A->data stack pointer

LABEL+2,
          LDA, 01234,
          GSBU, DEST+30, // A -> [DSP++] ( push data stack )
          GSBU, DEST+31, // [--DSP] -> A ( pop data stack )

          JMPU, DEST+2,

//*********** data stack subroutines ***********
// push data stack
LABEL+30,
          // push A on data stack
          STA+IND, DEST+20,
          // increment data stack pointer
          LDA+ABS, DEST+20,
          SAF,
          PLUS,0,
          STA+ABS, DEST+20,
          PLPC, // return

// pop data stack
LABEL+31,
          // decrement data stack pointer
          LDA+ABS, DEST+20,
          DEC, 0,
          STA+ABS, DEST+20,
          // data stack content -> A
          LDA+IND, DEST+20,

          PLPC, // return
};

/***********************************************************************

 program loader

   This routines copies the opcodes into the cpu memory.
   If the code of the program is a label, it is not copied.
   Instead a label entry is made.

 ***********************************************************************/
void loadProgram(uint16_t *dest)
{
  uint16_t n;
  uint16_t length,index=0,tmp,tmp1,labelIndex=0;
  length=sizeof(assemblerProgram)/sizeof(assemblerProgram[0]);
  // copy program code
  for(n=0;n<length;n++)
  {
    tmp=assemblerProgram[n];
    // check for label
    if((tmp&0xF000)==LABEL)
    {
      tmp1=tmp&0xFFF;
      if(tmp1<MAXNUMBEROFLABELS) Labels[tmp1]=index;
      else SYSTEMOUT("error: label out of range");
      labelIndex++;
    }else // if no label than copy code
    {
      dest[index++]=assemblerProgram[n];
    }
  }
  // replace destinations by label contents
  for(n=0;n<index;n++)
  {
    tmp=dest[n];
    if((tmp&0xF000)==DEST) dest[n]=Labels[(tmp&0xFFF)];
  }
  // show labels
  SYSTEMOUT("labels ");
  for(n=0;n<10;n++)printf("label %d, dest %d ",n,Labels[n]);

  // show memory
  SYSTEMOUT("memory:");

  for(n=0;n<index;n++)
  {
    SYSTEMOUTOCT("",dest[n]);
  };    SYSTEMOUTCR;
}

