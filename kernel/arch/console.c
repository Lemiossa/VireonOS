/*
* arch/console.c
* Created by Matheus Leme Da Silva
*/
#include <console.h>
#include <types.h>

int current_attr = 0x07;
u8 current_fg = 7;
u8 current_bg = 0;
u8 bright_fg = 0;

const u8 ansi_to_bios[] = {
  0,
  4,
  2,
  6,
  1,
  5,
  3,
  7
};

static void update_attr()
{
  u8 fg = current_fg;
  if (bright_fg && !(fg & 8)) {
    fg |= 8;
  }
  current_attr = fg | (current_bg << 4);
}

static void process_single_code(int num)
{
  if (num == 0) {
    current_fg = 7;
    current_bg = 0;
    bright_fg = 0;
  } else if (num == 1) {
    bright_fg = 1;
  } else if (num == 22) {
    bright_fg = 0;
  } else if (num >= 30 && num <= 37) {
    current_fg = ansi_to_bios[num - 30];
  } else if (num >= 90 && num <= 97) {
    current_fg = ansi_to_bios[num - 90] | 8;
  } else if (num >= 40 && num <= 47) {
    current_bg = ansi_to_bios[num - 40];
  } else if (num >= 100 && num <= 107) {
    current_bg = ansi_to_bios[num - 100] | 8;
  }
  update_attr();
}

static void process_ansi_code(const char *code)
{
  int num = 0;
  const char *ptr = code;
  while(*ptr) {
    num = 0;

    while(*ptr >= '0' && *ptr <= '9') {
      num = num * 10 + (*ptr - '0');
      ptr++;
    }

    process_single_code(num);

    if (*ptr == ';') {
      ptr++;
    } else {
      break;
    }
  }
}

void process_ansi_escape(const char *str, size_t *pos)
{
  size_t start_pos;
  if(str[*pos] != '\033') return;

  (*pos)++;

  if (str[*pos] != '[') return;

  (*pos)++;
  start_pos = *pos;

  while(str[*pos] != '\0' && str[*pos] != 'm') {
    (*pos)++;
  }

  if (str[*pos] == 'm') {
    char code_buffer[32];
    size_t len = *pos - start_pos;

    if (len < sizeof(code_buffer)-1) {
      size_t i;
      for( i = 0; i < len; i++) {
        code_buffer[i] = str[start_pos + i];
      }
      code_buffer[len] = '\0';
      process_ansi_code(code_buffer);
    }
    pos++;
  }
  return;
}
