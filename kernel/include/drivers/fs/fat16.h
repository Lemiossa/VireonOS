#ifndef FAT16_H
#define FAT16_H

#include <types.h>

struct fat16_bpb {
  u8 jump[3];
  u8 oem[8];
  u16 bytes_per_sec;
  u8 sec_per_clus;
  u16 reserved_secs;
  u8 fat_count;
  u16 root_entries;
  u16 total_secs;
  u8 media;
  u16 fat_size;
  u16 secs_per_track;
  u16 heads;
  u32 hidden;
  u32 large_secs;
};

struct fat16_dirent {
  u8 name[11];
  u8 attr;
  u8 reserved[10];
  u16 time;
  u16 date;
  u16 cluster;
  u32 size;
};

struct fat16_fs {
  u16 root_start;
  u16 root_size;
  u16 data_start;
  u16 fat_start;
  u8 sec_per_clus;
  u16 fat_size;
};

int init_fat16(void);
int fat16_find(char *name, struct fat16_dirent *dirent);
int fat16_read(struct fat16_dirent *dirent, void *buf, u16 max_bytes);
int fat16_list(void);

#endif /* FAT16_H */
