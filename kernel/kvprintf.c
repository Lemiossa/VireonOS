/*
* kvprintf.c
* Created by Matheus Leme Da Silva
*/
#include <console.h>
#include <arg.h>
#include <types.h>
#include <kernel.h>
#include "print_utils.h"

int kvprintf(const char *format, va_list args)
{
  int count=0;
  char *str_arg;
  int int_arg;
  long long_arg;
  ulong ulong_arg;
  char char_arg;
  uint hex_arg;
  size_t i=0;
  int width=0;
  char length_mod=0;
  while (format[i]!='\0') {
    if (format[i]=='%') {
      i++;
      width=0;
      length_mod=0;
      while (format[i]>='0' && format[i]<='9') {
        width=width*10+(format[i]-'0');
        i++;
      }
      if(format[i]=='l') {
        length_mod='l';
        i++;
      } else if (format[i]=='h') {
        length_mod='h';
        i++;
      }
      switch(format[i]) {
        case 'i':
        case 'd': {
          if(length_mod=='l') {
            long_arg=va_arg(args, long);
            print_long(long_arg);
          } else {
            int_arg=va_arg(args, int);
            print_int(int_arg);
          }
          count++;
        } break;
        case 'u': {
          if(length_mod=='l') {
            ulong_arg=va_arg(args, ulong);
            print_ulong(ulong_arg);
          } else {
            int_arg=va_arg(args, uint);
            print_uint(int_arg);
          }
          count++;
        } break;
        case 'x': {
          if(length_mod=='l') {
            ulong_arg=va_arg(args,ulong);
            print_hex_long(ulong_arg,width);
          } else {
            hex_arg=va_arg(args, uint);
            if (width==0)width=4;
            print_hex(hex_arg, width);
          }
          count+=width;
        } break;
        case 'p': {
          void *p=va_arg(args, void*);
          print_hex((uint)(ptr_t)p, 4);
          count+=4;
        } break;
        case 'c': {
          char_arg=(char)va_arg(args, int);
          vga_putc(char_arg, current_attr);
          count++;
        } break;
        case 's': {
          str_arg=va_arg(args, char*);
          if (!str_arg)str_arg="(null)";
          while (*str_arg) {
            vga_putc(*str_arg++, current_attr);
            count++;
          }
        } break;
        case '%': {
          vga_putc('%', current_attr);
          count++;
        } break;
        default: {
          vga_putc('%', current_attr);
          vga_putc(format[i], current_attr);
          count+=2;
        } break;
      }
    } else if (format[i]=='\033') {
      size_t pos=i;
      process_ansi_escape(format, &pos);
      i=pos+1;
      continue;
    } else if (format[i]=='\t') {
      int j;
      for(j=0;j<4;j++) {
        vga_putc(' ', current_attr);
        count++;
      }
    } else {
      vga_putc(format[i], current_attr);
      count++;
    }
    i++;
  }
  return count;
}
