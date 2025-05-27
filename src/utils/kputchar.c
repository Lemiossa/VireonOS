/*
* utils/kputchar.c
* Created by Matheus Leme Da Silva
*/

#include "types.h"
#include "utils.h"

void kputchar(uchar c)
{
  vga_putc(c, current_attr);
}
