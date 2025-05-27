/*
* kernel.c
* Created by Matheus Leme Da Silva
*/
#include "types.h"
#include "drivers/vga.h"
#include "utils.h"

#define TICK_MS 55 /* for 18.2 hz */
#define SCREEN_WID 80
#define SCREEN_HEI 50

char *disk="hd0";
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

void dettect_disk(u8 disk_drive)
{
  switch(disk_drive) {
    case 0x00: {
      disk="fd0";
    } break;
    case 0x01: {
      disk="fd1";
    } break;
    case 0x80: {
      disk="hd0";
    } break;
    case 0x81: {
      disk="hd1";
    } break;
    default: {
      disk="not identified!";
    } break;
  }
}

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

void kmain()
{
  init_vga();
  dettect_disk(bootdrive);
  kprintf("dettected %s\n", disk);

  print_sad();

  set_cursor(0, 1);

  enable_cursor();

  for(;;);
}
