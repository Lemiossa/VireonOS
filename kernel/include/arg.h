#ifndef ARG_H
#define ARG_H

typedef char *va_list;

#define __va_align(type) \
    (sizeof(type)>sizeof(int)?sizeof(int):sizeof(type))

#define va_start(ap, last) \
    ((ap)=(va_list)&(last)+sizeof(last))

#define va_arg(ap, type) \
    (*(type*)((ap)=(char*)((((unsigned int)(ap))+__va_align(type)-1) \
    & ~(__va_align(type)-1))+sizeof(type),(ap)-sizeof(type)))

#define va_end(ap) \
    ((void)0)

#endif /* ARG_H */
