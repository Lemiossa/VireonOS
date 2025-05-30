/*
* utils/format_str.c
* Created by Matheus Leme Da Silva
*/
#include "types.h"
#include "utils.h"

#define __fmt_buf_size 16

void format_str(uchar *fmt, uint *args, void (*putchar)(uchar), bool ansi)
{
  uchar digit[__fmt_buf_size];
  while(*fmt) {
    if(*fmt=='%') {
      uint value=0;
      uint base=0, width=0;
      int is_negative=0, use_uppercase=0;
      uchar *s;
      bool is_long=false;
      fmt++;
      while(*fmt>='0'&&*fmt<='9') {
        width=width*10+(*fmt-'0');
        fmt++;
      }
      if(*fmt=='l') {
        is_long=true;
        fmt++;
      }
      if(*fmt=='d'||*fmt=='i') {
        value=*args++;
        base=10;
        is_negative=(value<0)?1:0;
        if(is_negative)value=-(int)value;
      } else if (*fmt=='u') {
        value=*args++;
        base=10;
      } else if (*fmt=='x') {
        value=*args++;
        base=16;
        use_uppercase=0;
      } else if (*fmt=='X') {
        value=*args++;
        base=16;
        use_uppercase=1;
      } else if (*fmt=='p') {
        value=(uint)(*args++);
        base=16;
        width=4;
        use_uppercase=0;
      } else if(*fmt=='o') {
        value=*args++;
        base=8;
      } else if(*fmt=='b') {
        value=*args++;
        base=2;
      } else if(*fmt=='s') {
        s=(uchar*)(*args++);
        if(!s)s=(uchar*)"(null)";
        while(*s) {
          if(*s=='\033'&&ansi) {
            size_t pos=0;
            process_ansi_escape((char*)s, &pos);
            s+=pos+1;
          } else {
            (*putchar)(*s++);
          }
        }
      } else if(*fmt=='c') {
        (*putchar)((uchar)(*args++));
      } else if(*fmt=='%') {
        (*putchar)('%');
      } else {
        (*putchar)('%');
        if(*fmt)(*putchar)(*fmt);
      }
      if(base) {
        uint n=0;
        uint d;

        digit[__fmt_buf_size-1]='\0';
        if(value==0) {
          digit[__fmt_buf_size-2]='0';
          n=1;
        } else {
          do {
            d=value%base;
            digit[(__fmt_buf_size-1)-(++n)]=(d<=9)?('0'+d):(use_uppercase?('A'+d-10):('a'+d-10));
            value/=base;
          } while(value&&n<(__fmt_buf_size-1));
        }
        while(n<width&&n<(__fmt_buf_size-1))digit[(__fmt_buf_size-1)-(++n)]='0';
        if(is_negative&&n<(__fmt_buf_size-1))digit[(__fmt_buf_size-1)-(++n)]='-';
        s=&digit[(__fmt_buf_size-1)-n];
        while(*s)(*putchar)(*s++);
      }
      fmt++;
    } else {
      if(*fmt=='\033'&&ansi){
        size_t pos=0;
        uchar *fmt_start=fmt;
        process_ansi_escape((char*)fmt, &pos);
        fmt=fmt_start+pos+1;
      } else {
        (*putchar)(*fmt++);
      }
    }
  }
}
