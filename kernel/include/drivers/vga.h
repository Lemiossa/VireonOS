#ifndef VGA_H
#define VGA_H
void vga_putc(char c, int attr);
void init_vga();
void set_cursor(uint x, uint y);
#endif /* VGA_H */
