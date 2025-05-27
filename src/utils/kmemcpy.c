/*
* utils/kmemcpy.c
* Created by Matheus Leme Da Silva
*/
#include "types.h"
#include "utils.h"

void *kmemcpy(void *dest, void *src, size_t n)
{
  uchar *d=(uchar*)dest;
  uchar *s=(uchar*)src;
  while(n--)*d++=*s++;
  return dest;
}
