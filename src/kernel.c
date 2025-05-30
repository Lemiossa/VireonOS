/*
* kernel.c
* Created by Matheus Leme Da Silva
*/
#include "types.h"
#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "utils.h"

#define TICK_MS 55 /* for 18.2 hz */

extern uint ticks;
extern uchar bootdrive;

#define OPTION_COUNT 3

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

void debug(uchar *msg)
{
  kprintf("[%5u] %s", get_ms(), msg);
}

void kmain()
{
  init_vga();
  debug("initializing disk...\n");
  kprintf("\tDrive: 0x%2x|0b%8b\n", bootdrive, bootdrive);
  if(!init_disk(bootdrive)) debug("initialized!\n"); else panic("Failed to initialize disk");

  halt();
}
