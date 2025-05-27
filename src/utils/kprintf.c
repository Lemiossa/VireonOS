/*
* utils/kprintf.c
* Created by Matheus Leme Da Silva
*/

#include "types.h"
#include "utils.h"

void kprintf(uchar *format, ...)
{
  format_str(format, (uint*)(&format+1), kputchar, true);
}
