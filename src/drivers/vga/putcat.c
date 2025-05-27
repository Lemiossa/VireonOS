/*
* arch/drivers/vga/putcat.c
* Created by Matheus Leme Da Silva
*/
#include "drivers/vga.h"
#include "types.h"

/*
  * putchar at position
*/
void putcat(uint x, uint y, uchar c, uint attr)
{
  set_cursor(x, y);
  vga_putc(c, attr);
}
