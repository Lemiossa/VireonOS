#ifndef CONSOLE_H
#define CONSOLE_H
#include <types.h>
extern int current_attr;
void process_ansi_escape(const char *str, size_t *pos);
void handle_ansi_code(int code);
#endif /* CONSOLE_H */
