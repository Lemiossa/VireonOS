/*
* kprintf.c
* Created by Matheus Leme Da Silva
*/

#include <arg.h>
#include <types.h>

int kprintf(const char *format, ...)
{
  int count;
  va_list args;

  va_start(args, format);
  count = kvprintf(format, args);
  va_end(args);

  return count;
}
