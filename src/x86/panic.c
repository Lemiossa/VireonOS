/*
* x86/panic.c
* Created by Matheus Leme Da Silva
*/
#include "types.h"
#include "utils.h"
#include "panic.h"
#include "drivers/vga.h"

#define SCREEN_WID 80
#define SCREEN_HEI 50

void print_sad()
{
  int center_x=SCREEN_WID-12;
  int center_y=SCREEN_HEI-12;
  disable_cursor();
  putcat(center_x-3,  center_y-2, ' ', 0xff);
  putcat(center_x+3,  center_y-2, ' ', 0xff);
  putcat(center_x-3,  center_y-1, ' ', 0xff);
  putcat(center_x+3,  center_y-1, ' ', 0xff);
  putcat(center_x-3,  center_y+4, ' ', 0xff);
  putcat(center_x-2,  center_y+3, ' ', 0xff);
  putcat(center_x-1,  center_y+2, ' ', 0xff);
  putcat(center_x,    center_y+2, ' ', 0xff);
  putcat(center_x+1,  center_y+2, ' ', 0xff);
  putcat(center_x+2,  center_y+3, ' ', 0xff);
  putcat(center_x+3,  center_y+4, ' ', 0xff);
}

void print_panic_info(panic_info_t* info)
{
  clear_screen(0x4f);
  print_sad();
  set_cursor(0, 0);
  kprintf("\033[1;37;41m\nOops, an error ocurred!\n\n");
  kprintf("Error: %s\n\n", info->msg);
  kprintf("  AX = %4x",         info->regs.ax);
  kprintf("  BX = %4x",         info->regs.bx);
  kprintf("  CX = %4x",         info->regs.cx);
  kprintf("  DX = %4x\n",       info->regs.dx);
  kprintf("  SI = %4x",         info->regs.si);
  kprintf("  DI = %4x",         info->regs.di);
  kprintf("  BP = %4x",         info->regs.bp);
  kprintf("  SP = %4x\n",       info->regs.sp);
  kprintf("  CS = %4x",         info->regs.cs);
  kprintf("  DS = %4x",         info->regs.ds);
  kprintf("  ES = %4x",         info->regs.es);
  kprintf("  SS = %4x\n",       info->regs.ss);
  kprintf("  IP = %4x",         info->regs.ip);
  kprintf("  FLAGS = %4x\n\n",  info->regs.flags);
  kprintf("Stack Trace: \n");
  if(info->frame_count==0) {
    kprintf("  No stack frames captured\n");
  } else {
    u8 i;
    for (i=0;i<info->frame_count;i++) {
      kprintf("  [%d] %4x:%4x\n", i, info->stack_frames[i].seg, info->stack_frames[i].offset);
    }
  }
  for(;;);
}

void panic(uchar *message)
{
  panic_info_t info;
  info.msg=message;
  info.frame_count=0;
  capture_registers(&info.regs);
  capture_stack_trace(info.stack_frames, &info.frame_count);
  print_panic_info(&info);
  for(;;);
}
