/*
* utils/kputs.c
* Created by Matheus Leme Da Silva
*/

#include "types.h"
#include "utils.h"

void kputs(uchar *s)
{
  while(*s) {
    kputchar(*s++, current_attr);
  }
}
