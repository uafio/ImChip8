#pragma once
#include <stdint.h>

#define LOG( ... ) printf( __VA_ARGS__ )
#define EPRINT( msg ) std::perror( msg )
#define X( v ) ( ( (v)&0x0f00 ) >> 8 )
#define Y( v ) ( ( (v)&0x00f0 ) >> 4 )
#define Z( v ) ( ( (v)&0x000f ) )
#define NNN( v ) ( (v)&0x0fff )
#define KK( v ) ( (v)&0x00ff )

typedef struct _CTX {
    uint16_t PC;
    int8_t SP;
    uint8_t V[16];
    uint16_t I;
    uint8_t DT;
    uint8_t ST;
} CTX;
