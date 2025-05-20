/*
* kernel.c
* Created by Matheus Leme Da Silva
*/
#include <types.h>
#include <kernel.h>
#include <drivers/vga.h>
#include "print_utils.h"

#define TICK_MS 52

extern uint ticks;
extern uchar bootdrive;

uint get_ms()
{
  return ticks*TICK_MS;
}

void delay_ms(uint ms)
{
  int ttw=(ms+(int)(TICK_MS)-1)/(int)(TICK_MS);
  int st=ticks;
  int current_tick;
  do {
    current_tick=ticks;
  } while ((current_tick-st)<ttw);
}

void halt()
{
  for (;;);
}

void kmain()
{
  init_vga();

  if (bootdrive==0x00)kprintf("dettected fd0\n");
  if (bootdrive==0x01)kprintf("dettected fd1\n");
  if (bootdrive==0x80)kprintf("dettected hd0\n");
  if (bootdrive==0x81)kprintf("dettected hd1\n");

  kprintf("Current MS: %u\n", get_ms());

  for(;;)
  {
    kprintf(" %u MS", get_ms());
  }
}
