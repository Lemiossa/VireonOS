#ifndef PANIC_H
#define PANIC_H

#include "types.h"
#define MAX_STACK_FRAMES 10

typedef struct {
  u16 ax, bx, cx, dx;
  u16 si, di, bp, sp;
  u16 cs, ds, es, ss;
  u16 ip, flags;
} regs_t;

typedef struct {
  u16 seg;
  u16 offset;
} stkf_t;

typedef struct {
  uchar *msg;
  regs_t regs;
  stkf_t stack_frames[MAX_STACK_FRAMES];
  u8 frame_count;
} panic_info_t;

void panic(uchar *message);

void print_panic_info(panic_info_t *info);
void capture_register(regs_t *regs);
void capture_stack_trace(stkf_t *frames, u8 *count);

#endif /* PANIC_H */
