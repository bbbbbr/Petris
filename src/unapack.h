#ifndef __UNAPACK_H_INCLUDE
#define __UNAPACK_H_INCLUDE

#ifndef OLDCALL
  #if __SDCC_REVISION >= 12608
    #define OLDCALL __sdcccall(0)
  #else
    #define OLDCALL
  #endif
#endif

void UNAPACK(const unsigned char * sour, unsigned char * dest) OLDCALL;

#endif
