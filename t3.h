#ifndef __T3__
  #define __T3__

  #include <stdint.h>
  /*
     ftp://ftp.dreesen.ch/

     The T3 is a computer with a 12 bit T3 cpu.
     There are two ram memories with a size of 4096x12bit.
     There is also a separated stack with 256x12bit
     Due to the fact that the T3 is a 12 bit computer,
     octal notation is better suited for machine instructions
     than hexadecimal notation

     remark: octal notation in C is made by a preceding '0'
     example 012 octal is equal to 0xA hex
   */
  /*
   ============================================================================
   Name        : t3.h
   Author      : chris
   Version     :
   Copyright   : GPL lizense 3
                 ( chris (at) roboterclub-freiburg.de )
   Description : t3 virtual machine definitons in C, Ansi-style
   ============================================================================
   */

  #define M_SIZE 4096
  #define STACK_SIZE 256
  #define BANKS 2

  typedef struct {
    uint16_t M[BANKS][M_SIZE]; // memory
    uint16_t stack[STACK_SIZE];
    uint16_t A;
    uint16_t Sp;
    uint16_t Pc; // program counter
    uint16_t flags;
    uint8_t  bank; // bank 0 or 1
    uint16_t display; // 4 digit nixie tube display ( panel output P )
    uint16_t keys;    // 12 input keys ( panel input P )
    uint16_t inport;  // 12 bit user input lines ( IO )
    uint16_t outport; // 12 bit user output lines ( IO )
  }Cpu_t;
  /* function prototypes */
  void simulatorReset(Cpu_t *cpu);
  void showCpu(Cpu_t *cpu);
  void executeVm(Cpu_t *cpu);
  uint16_t readMemory(Cpu_t *cpu, uint16_t mode);
  void writeMemory(Cpu_t *cpu, uint16_t mode,uint16_t value);
  void push(Cpu_t *cpu,uint16_t value);
  uint16_t pop(Cpu_t *cpu);

  // flags
  #define EQ_FLAG      0  // A = OPR
  #define GT_FLAG      1  // A > OPR
  #define SM_FLAG      2  // A < OPR
  #define ALU_FLAG     3  //
  #define MSB_FLAG     4  // MSB of A
  #define SF_FLAG      5  // shift flag
  #define AF_FLAG      6  // accu flag
  #define NIL_FLAG     7  // always 0
  #define SKF_FLAG     8  // "store" key
  #define IKF_FLAG     9  // "inc" key
  #define EFA_FLAG     10 // external flag A
  #define EFB_FLAG     11 // external flag B
  #define STROBE_FLAG  12
  #define ONE1_FLAG    13 // always set to 1
  #define ONE_FLAG     14 // always set to 1, may be used for unconditional jump
  #define HZ50_FLAG    15 // 50Hz timer signal

  // abbreviations
  // PC  programm counter
  // I/O IO-register
  // SP  stack pointer
  // OPR operand
  // A   accumulator
  // P   panel ( the panel consist of the keys and the display
  //             it can be read or written, read: keys, write: display )
  // M   addressed word in memory
  // N   integer 0..7 ( number of bits to be shift )

  /*
    addressing modes
    IMM: immediate second word is operand
    ABS: second word is address of operand
    IND: second word is address of address of operand
  */
  #define IMM  00000
  #define ABS  01000
  #define IND  02000

  /*
    examples:
    LDA 0123       0123->A
    LDA+ABS 0100   memory[0100]->A
    LDA+IND 0100   memory[memory[0100]]->A

    the indirect addressing mode is useful for generation virtual pointers
    e.g. we want to make an data stack pointer
    The data stack pointer shall be located at address 0400 and shall point to
    the data stack at 0500

  // initialize data stack pointer
    LDA     0500 // 500->A
    STA+ABS 0400 // store 0500 at the address 0x400

  // push A
    STA_IND 0400 // A->memory[memory[400]]
    // decrement data stack pointer
    LDA+ABS 0400
    DEC+ABS 0
    STA+ABS 0400

  //pop A
    // increment data stack pointer
    LDA+ABS 0400
    SAF         // set carry flag
    PLUS 0
    STA+ABS 0400
    // load content
    LDA+IND 0400
  */

  // ALU instructions
  // two word instructions
  // instruction with addressing mode, operand
  #define NOP2   00000 // A->A nop with 2 instructions
  #define MINUS  00006 // A-M-1 -> A
  #define PLUS   00011 // A+M -> A
  #define DOUBLE 00014 // A*2 -> A, operand not used
  #define DEC    00017 // A-1 -> A, operand not used
  #define INV    00020 // NOT A -> A, operand not used
  #define NOR    00021 // A NOR M -> A
  #define ZERO   00023 // 0 -> A -> A
  #define NAND   00024 // A NAND M -> A
  #define INVM   00025 // NOT M -> A
  #define EXOR   00026 // A EXOR M -> A
  #define EXNOR  00031 // A EXNOR M -> A
  #define AND    00033 // A and M -> A
  #define ONES   00034 // 0777->A set all ones
  #define OR     00036 // A or M -> A

  // program flow ( conditional jumps and calls )
  // the lower 4 bits are used for flag selection
  // BA9876543210 ==> bit number in hex
  // AAACCC01ffff ==> addressing mode, command, flag number
  #define JMPMASK  00760
  #define FLAGMASK 00017
  #define GSBS   00300 // gosub if flag is set ( PC->STCK; M->PC if flag set)
  #define GSBR   00320 // gosub if flag is reset ( PC->STCK; M->PC if flag reset)
  #define JMPS   00340 // jump if flag is set ( M->PC if flag set )
  #define JMPR   00360 // jump if flag is reset ( M->PC if flag reset )

  #define JMPU (JMPR+NIL_FLAG) // jump unconditional
  #define GSBU  (GSBR+NIL_FLAG) // gosub unconditional

  // machine control
  #define RST    00400 // 0001->PC
  #define STOP   00401 // stop
  #define RSD    00402 // reset display
  #define SHIB   00403 // select high bank
  #define SLOB   00404 // select low bank
  #define RSSP   00405 // 0->SP, reset stack pointer
  #define RSA    00406 // 0->ACCU, reset accumulator
  #define STB    00407 // strobe ?
  #define SAF    00410 // set accu flag ( AF_FLAG )
  #define CAF    00411 // clear accu flag ( AF_FLAG )
  #define SSF    00412 // set shift flag ( SF_FLAG )
  #define CSF    00413 // clear shift flag ( SF_FLAG )
  #define NOP1   00414 // one word nop

  // stack operations
  #define PLPC   00600 // pull PC ( return )
  #define PLP    00601 // pull P
  #define PLIO   00602 // pull I/O
  #define PLA    00603 // pull A
  #define PHPC   00604 // push PC
  #define PHP    00614 // push P
  #define PHIO   00624 // push I/O
  #define PHA    00634 // push A

  // qrzForth compatibility
  #define RTS PLPC // return from subroutine

  // register memory transfer
  #define STPC   00100 // PC -> M
  #define STP    00110 // P -> M
  #define STIO   00120 // I/O -> M
  #define STA    00130 // A -> M
  #define LDPC   00104 // M -> PC
  #define LDP    00105 // M -> P
  #define LDIO   00106 // M -> I/O
  #define LDA    00107 // M -> A

  // compare instructions
  #define CMP    00200 // A-M -> FLG

  // register register transfer
  #define TPCP   00501 // PC -> P // PC -> display
  #define TPCIO  00502 // PC -> IO
  #define TPCA   00503 // PC -> A
  #define TPPC   00510 // P -> PC
  #define TPP    00511 // P -> P read keyboard, write display
  #define TPIO   00512 // P -> IO
  #define TPA    00513 // P -> A // keys -> A
  #define TIOPC  00520 // IO->PC
  #define TIOP   00521 // IO->P
  #define TIOA   00523 // IO->A
  #define TAPC   00530 // A->PC ( variable jump )
  #define TAP    00531 // A->P store A in display
  #define TAIO   00532 // A->IO store A in outport

  // shift operations ( n bits 0..7 )
  // example:
  // ROL+2 rotate left n bits
  #define ROL 00700 // rotate left n bits
  #define ROR 00710 // rotate right n bits
  #define SFL 00720 // shift left n bits
  #define SFR 00730 // shift right n bits

  #define SHIFTMASK 0770

#endif




