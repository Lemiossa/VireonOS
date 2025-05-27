#ifndef VGA_H
#define VGA_H
#include "types.h"
void vga_putc(uchar c, uint attr);
void init_vga();
void set_cursor(uint x, uint y);
void putcat(uint x, uint y, uchar c, uint attr);
void enable_cursor();
void disable_cursor();
uint get_cursor_x_position();
uint get_cursor_y_position();
#endif /* VGA_H */
