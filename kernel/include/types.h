#ifndef TYPES_H
#define TYPES_H

#define u   unsigned
#define sh  short
#define si  signed

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long int ulong;

typedef signed char schar;
typedef signed int sint;
typedef signed short sshort;
typedef signed long slong;

typedef uchar byte;
typedef ushort word;
typedef ulong dword;

typedef uchar u8;
typedef ushort u16;
typedef ulong u32;

typedef schar i8;
typedef sshort i16;
typedef slong i32;

typedef u16 ptr_t;
typedef uint size_t;
typedef uchar bool;
typedef uchar BOOL;

#define TRUE 1
#define FALSE 0
#define NULL ((void*)0)

#endif /* TYPES_H */
