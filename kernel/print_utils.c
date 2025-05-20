/*
* print_utils.c
* Created by Matheus Leme Da Silva
*/

#include "print_utils.h"
#include <types.h>
#include <console.h>
#include <drivers/vga.h>



void print_int(int num)
{
  char buffer[12];
  int i=0;
  int is_negative=0;
  if (num==0) {
    vga_putc('0', current_attr);
    return;
  }
  if (num<0) {
    is_negative = 1;
    num=-num;
  }
  while (num>0) {
    buffer[i++]='0'+(num%10);
    num/=10;
  }
  if (is_negative)vga_putc('-', current_attr);
  while (i>0)vga_putc(buffer[--i], current_attr);
}

void print_uint(uint num)
{
  char buffer[11];
  int i=0;
  if(num==0) {
    vga_putc('0', current_attr);
    return;
  }
  while(num>0) {
    buffer[i++]='0'+(num%10);
    num/=10;
  }
  while(i > 0)vga_putc(buffer[--i], current_attr);
}

void print_hex(uint num, int width)
{
  static const char hex_digits[]="0123456789abcdef";
  char buffer[16];
  int i=0;
  do {
    buffer[i++]=hex_digits[num&0xf];
    num>>=4;
  }while(num>0);
  while(i<width)buffer[i++]='0';
  while(i>0)vga_putc(buffer[--i],current_attr);
}

void print_hex_long(ulong num, int width)
{
  static const char hex_digits[]="0123456789abcdef";
  char buffer[16];
  int i=0;
  do {
    buffer[i++]=hex_digits[num&0xf];
    num>>=4;
  }while(num>0);
  while(i<width)buffer[i++]='0';
  while(i>0)vga_putc(buffer[--i],current_attr);
}

void print_long(long num)
{
  char buffer[22];
  int i=0;
  int is_negative=0;
  if(num==0){
    vga_putc('0', current_attr);
    return;
  }
  if(num<0){
    is_negative=1;
    num=-num;
  }
  while(num>0){
    buffer[i++]='0'+(num%10);
    num/=10;
  }
  if(is_negative)vga_putc('-', current_attr);
  while(i>0)vga_putc(buffer[--i], current_attr);
}

void print_ulong(ulong num)
{
  char buffer[21];
  int i=0;
  if(num==0){
    vga_putc('0', current_attr);
    return;
  }
  while(num>0){
    buffer[i++]='0'+(num%10);
    num/=10;
  }
  while(i>0)vga_putc(buffer[--i], current_attr);
}
