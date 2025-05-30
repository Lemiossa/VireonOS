#ifndef UTILS_H
#define UTILS_H
#include "types.h"
void *kmemcpy(void *dest, void *src, size_t n);
void kputchar(uchar c);
void kputs(uchar *str);
void format_str(uchar *fmt, uint *args, void (*putchar)(uchar), bool ansi);
void kprintf(uchar *format, ...);
extern int current_attr;
void process_ansi_escape(uchar *str, size_t *pos);
void handle_ansi_code(int code);
#endif /* UTILS_H */
