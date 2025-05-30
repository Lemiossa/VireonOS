#ifndef TYPES_H
#define TYPES_H

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef char s8;
typedef int s16;
typedef long s32;

typedef uchar u8;
typedef ushort u16;
typedef ulong u32;

typedef u16 ptr_t;
typedef uint size_t;
typedef uchar bool;
typedef uchar BOOL;

#define TRUE 1
#define true 1
#define FALSE 0
#define false 0
#define NULL ((void*)0)
#define null ((void*)0)

#endif /* TYPES_H */
