/*
 ============================================================================
 Name        : systemout.h
 Author      : chris
 Version     :
 Copyright   : GPL lizense 3
               ( chris (at) roboterclub-freiburg.de )
 Description : wrapper macros for message and debugging outputs
 ============================================================================
 */
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __SYSTEMOUT__
  #define __SYSTEMOUT__
	//#include "platform.h"

	// #define DEBUGCPU

		#ifdef DEBUGCPU
			#define CPU_DEBUGOUT(text) { puts(text);};
			#define CPU_DEBUGOUT_(text) { printf(text);};
			#define CPU_DEBUGOUTHEX(text,value) { printf(text);printf(" %x\n",value);};
		#else
			#define CPU_DEBUGOUT(text)
			#define CPU_DEBUGOUT_(text)
			#define CPU_DEBUGOUTHEX(text,value)
		#endif

		#define SYSTEMGETCHAR() getchar()
        #define SYSTEMKEYPRESSED() 1 // on a PC we can not ask for dynamic keys
        #define SYSTEMGETKEY() getchar()
		#define SYSTEMOUT(text) { puts(text);};
		#define SYSTEMOUT_(text) { printf("%s",text);};
		#define SYSTEMOUTCR {puts("");};
		#define SYSTEMOUTHEX(text,value) { printf("%s",text);printf("%04x ",value);};
		#define SYSTEMOUTHEX2(text,value) { printf(text);printf("%02x ",value);};
		#define SYSTEMOUTDEC(text,value) { printf(text);printf("%d ",value);};
		#define SYSTEMOUTCHAR(value) { putchar(value);};
		#define SYSTEMOUTPRINTNUMBER(value) { printf(" %d ",value);};
		#define SYSTEMOUTPRINTHEXNUMBER(value) { printf(" %4x ",value);};

        #define DISASM(str) SYSTEMOUT(str)
        #define ERROR(str,value) { printf("%s %04o ",str,value);};

#endif // __SYSTEMOUT__

#ifdef __cplusplus
}
#endif

