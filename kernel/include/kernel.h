#ifndef KERNEL_H
#define KERNEL_H

#include <arg.h>

void kmain();
int kprintf(const char *format, ...);
int kvprintf(const char *format, va_list args);

#endif /* KERNEL_H */
