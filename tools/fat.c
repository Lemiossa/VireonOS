/**
 * Mini FAT Debugger - A lightweight CLI tool for FAT12/FAT16 filesystem
 * analysis
 *
 * Supports:
 * - Reading FAT12 and FAT16 filesystems
 * - Displaying filesystem information
 * - Browsing directories
 * - Examining file details
 * - Viewing file content in hex/ascii
 */

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI color codes for interface
#define COLOR_RESET "\x1B[0m"
#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN "\x1B[36m"
#define COLOR_WHITE "\x1B[37m"
#define BOLD "\x1B[1m"

// FAT constants
#define SECTOR_SIZE 512
#define FAT_ENTRY_SIZE_12 1.5
#define FAT_ENTRY_SIZE_16 2
#define DIR_ENTRY_SIZE 32
#define MAX_PATH_LEN 260

// FAT BPB (BIOS Parameter Block) structure
typedef struct {
  uint8_t jump_boot[3];
  char oem_name[8];
  uint16_t bytes_per_sector;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sectors;
  uint8_t num_fats;
  uint16_t root_entries;
  uint16_t total_sectors_16;
  uint8_t media_descriptor;
  uint16_t fat_size_16;
  uint16_t sectors_per_track;
  uint16_t num_heads;
  uint32_t hidden_sectors;
  uint32_t total_sectors_32;
  // Extended BPB
  uint8_t drive_number;
  uint8_t reserved;
  uint8_t boot_signature;
  uint32_t volume_id;
  char volume_label[11];
  char fs_type[8];
} __attribute__((packed)) BPB;

// FAT directory entry structure
typedef struct {
  char name[8];
  char ext[3];
  uint8_t attributes;
  uint8_t reserved[10];
  uint16_t time;
  uint16_t date;
  uint16_t starting_cluster;
  uint32_t file_size;
} __attribute__((packed)) DirEntry;

// Global filesystem state
typedef struct {
  FILE *fp;
  BPB bpb;
  uint8_t fat_type; // 12 or 16
  uint32_t fat_start_offset;
  uint32_t root_dir_start_offset;
  uint32_t data_start_offset;
  uint32_t current_dir_cluster; // 0 for root
  char current_path[MAX_PATH_LEN];
} FatState;

// Function prototypes
void initialize_fat_state(FatState *state);
bool open_image(const char *filename, FatState *state);
void display_fat_info(FatState *state);
uint32_t get_fat_entry(FatState *state, uint32_t cluster);
void list_directory(FatState *state, uint32_t cluster);
void display_file_content(FatState *state, uint32_t cluster, uint32_t size);
void print_help();
void clean_up(FatState *state);

// Main program
int main(int argc, char *argv[]) {
  FatState state;
  char command[256];
  bool running = true;

  // Check if an image file was provided
  if (argc < 2) {
    printf("%s%sUsage: %s <fat_image_file>%s\n", BOLD, COLOR_YELLOW, argv[0],
           COLOR_RESET);
    return 1;
  }

  // Initialize and open the FAT image
  initialize_fat_state(&state);
  if (!open_image(argv[1], &state)) {
    printf("%s%sError opening FAT image file:%s %s\n", BOLD, COLOR_RED,
           COLOR_RESET, argv[1]);
    printf("  %s  Reason: %s%s\n", COLOR_YELLOW, strerror(errno), COLOR_RESET);
    printf("  %s  Make sure the file exists and you have read permissions%s\n",
           COLOR_YELLOW, COLOR_RESET);
    return 1;
  }

  // Display initial FAT information
  display_fat_info(&state);
  strcpy(state.current_path, "/");

  // Main command loop
  while (running) {
    printf("%s%s%s> %s", BOLD, COLOR_GREEN, state.current_path, COLOR_RESET);
    if (!fgets(command, sizeof(command), stdin)) {
      break;
    }

    // Remove newline
    command[strcspn(command, "\n")] = 0;

    // Process command
    if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
      running = false;
    } else if (strcmp(command, "info") == 0) {
      display_fat_info(&state);
    } else if (strcmp(command, "ls") == 0 || strcmp(command, "dir") == 0) {
      list_directory(&state, state.current_dir_cluster);
    } else if (strncmp(command, "cd ", 3) == 0) {
      // Change directory - simplified for example
      printf("Changing directory not implemented in this mini version\n");
    } else if (strncmp(command, "cat ", 4) == 0) {
      // Display file content - simplified for example
      printf("File viewing not implemented in this mini version\n");
    } else if (strcmp(command, "help") == 0) {
      print_help();
    } else if (strlen(command) > 0) {
      printf("%sUnknown command: %s%s\n", COLOR_RED, command, COLOR_RESET);
    }
  }

  clean_up(&state);
  return 0;
}

// Initialize the FAT state structure
void initialize_fat_state(FatState *state) {
  memset(state, 0, sizeof(FatState));
}

// Open and analyze the FAT image file
bool open_image(const char *filename, FatState *state) {
  state->fp = fopen(filename, "rb");
  if (!state->fp) {
    return false;
  }

  // Read the boot sector
  if (fread(&state->bpb, sizeof(BPB), 1, state->fp) != 1) {
    fprintf(stderr, "%sError reading boot sector: %s%s\n", COLOR_RED,
            strerror(errno), COLOR_RESET);
    fclose(state->fp);
    return false;
  }

  // Basic validation - check signature bytes
  if (state->bpb.bytes_per_sector == 0 ||
      (state->bpb.bytes_per_sector != 512 &&
       state->bpb.bytes_per_sector != 1024 &&
       state->bpb.bytes_per_sector != 2048 &&
       state->bpb.bytes_per_sector != 4096)) {
    fprintf(stderr,
            "%sInvalid FAT image: Bytes per sector is %d (expected 512, 1024, "
            "2048, or 4096)%s\n",
            COLOR_RED, state->bpb.bytes_per_sector, COLOR_RESET);
    fclose(state->fp);
    return false;
  }

  if (state->bpb.sectors_per_cluster == 0 ||
      (state->bpb.sectors_per_cluster & (state->bpb.sectors_per_cluster - 1)) !=
          0) {
    fprintf(stderr,
            "%sInvalid FAT image: Sectors per cluster is %d (must be a power "
            "of 2)%s\n",
            COLOR_RED, state->bpb.sectors_per_cluster, COLOR_RESET);
    fclose(state->fp);
    return false;
  }

  // Detect FAT type (12 or 16)
  uint32_t total_sectors = state->bpb.total_sectors_16;
  if (total_sectors == 0) {
    total_sectors = state->bpb.total_sectors_32;
  }

  if (total_sectors == 0) {
    fprintf(stderr, "%sInvalid FAT image: Total sectors is 0%s\n", COLOR_RED,
            COLOR_RESET);
    fclose(state->fp);
    return false;
  }

  uint32_t data_sectors =
      total_sectors - state->bpb.reserved_sectors -
      (state->bpb.num_fats * state->bpb.fat_size_16) -
      ((state->bpb.root_entries * 32 + state->bpb.bytes_per_sector - 1) /
       state->bpb.bytes_per_sector);

  uint32_t total_clusters = data_sectors / state->bpb.sectors_per_cluster;

  if (total_clusters < 4085) {
    state->fat_type = 12;
  } else {
    state->fat_type = 16;
  }

  // Calculate key offsets
  state->fat_start_offset =
      state->bpb.reserved_sectors * state->bpb.bytes_per_sector;
  state->root_dir_start_offset =
      state->fat_start_offset + (state->bpb.num_fats * state->bpb.fat_size_16 *
                                 state->bpb.bytes_per_sector);
  state->data_start_offset =
      state->root_dir_start_offset + (state->bpb.root_entries * DIR_ENTRY_SIZE);

  return true;
}

// Display FAT filesystem information
void display_fat_info(FatState *state) {
  char volume_label[12] = {0};
  char fs_type[9] = {0};

  memcpy(volume_label, state->bpb.volume_label, 11);
  memcpy(fs_type, state->bpb.fs_type, 8);

  printf("\n%s%s===== FAT Filesystem Information =====%s\n", BOLD, COLOR_CYAN,
         COLOR_RESET);
  printf("%sOEM Name:%s %.8s\n", COLOR_YELLOW, COLOR_RESET,
         state->bpb.oem_name);
  printf("%sVolume Label:%s %s\n", COLOR_YELLOW, COLOR_RESET, volume_label);
  printf("%sFilesystem Type:%s %s (Detected: FAT%d)\n", COLOR_YELLOW,
         COLOR_RESET, fs_type, state->fat_type);
  printf("%sBytes per Sector:%s %d\n", COLOR_YELLOW, COLOR_RESET,
         state->bpb.bytes_per_sector);
  printf("%sSectors per Cluster:%s %d\n", COLOR_YELLOW, COLOR_RESET,
         state->bpb.sectors_per_cluster);
  printf("%sReserved Sectors:%s %d\n", COLOR_YELLOW, COLOR_RESET,
         state->bpb.reserved_sectors);
  printf("%sNumber of FATs:%s %d\n", COLOR_YELLOW, COLOR_RESET,
         state->bpb.num_fats);
  printf("%sRoot Directory Entries:%s %d\n", COLOR_YELLOW, COLOR_RESET,
         state->bpb.root_entries);
  printf("%sTotal Sectors:%s %d\n", COLOR_YELLOW, COLOR_RESET,
         state->bpb.total_sectors_16 ? state->bpb.total_sectors_16
                                     : state->bpb.total_sectors_32);
  printf("%sFAT Size in Sectors:%s %d\n", COLOR_YELLOW, COLOR_RESET,
         state->bpb.fat_size_16);
  printf("\n");
}

// Get the FAT entry for a given cluster
uint32_t get_fat_entry(FatState *state, uint32_t cluster) {
  uint32_t fat_offset;
  uint32_t entry_value = 0;

  if (state->fat_type == 12) {
    fat_offset = cluster + (cluster / 2);

    // Read the 16-bit value containing our entry
    uint16_t fat_bytes;
    fseek(state->fp, state->fat_start_offset + fat_offset, SEEK_SET);
    fread(&fat_bytes, 2, 1, state->fp);

    // Extract proper 12-bit entry
    if (cluster & 0x1) {
      // Odd cluster
      entry_value = (fat_bytes >> 4) & 0xFFF;
    } else {
      // Even cluster
      entry_value = fat_bytes & 0xFFF;
    }
  } else {
    // FAT16
    fat_offset = cluster * 2;
    fseek(state->fp, state->fat_start_offset + fat_offset, SEEK_SET);
    fread(&entry_value, 2, 1, state->fp);
  }

  return entry_value;
}

// List directory contents
void list_directory(FatState *state, uint32_t cluster) {
  DirEntry dir_entry;
  uint32_t entry_count = 0;

  // For the root directory
  if (cluster == 0) {
    fseek(state->fp, state->root_dir_start_offset, SEEK_SET);
    int max_entries = state->bpb.root_entries;

    printf("\n%s%s%-12s %-4s %-8s %-19s %-8s%s\n", BOLD, COLOR_CYAN, "Name",
           "Attr", "Size", "Date/Time", "Cluster", COLOR_RESET);
    printf("%s%s--------------------------------------------------------%s\n",
           BOLD, COLOR_BLUE, COLOR_RESET);

    for (int i = 0; i < max_entries; i++) {
      if (fread(&dir_entry, sizeof(DirEntry), 1, state->fp) != 1) {
        break;
      }

      // Skip unused entries
      if (dir_entry.name[0] == 0x00) {
        // End of directory
        break;
      }

      if (dir_entry.name[0] == 0xE5) {
        // Deleted entry
        continue;
      }

      // Format filename
      char filename[13];
      int j, k;

      // Copy name (trim trailing spaces)
      for (j = 0; j < 8 && dir_entry.name[j] != ' '; j++) {
        filename[j] = dir_entry.name[j];
      }

      // Add dot and extension if exists
      if (dir_entry.ext[0] != ' ') {
        filename[j++] = '.';
        for (k = 0; k < 3 && dir_entry.ext[k] != ' '; k++) {
          filename[j + k] = dir_entry.ext[k];
        }
        filename[j + k] = '\0';
      } else {
        filename[j] = '\0';
      }

      // Print directory entry information
      char attr_str[5] = "----";
      if (dir_entry.attributes & 0x01)
        attr_str[0] = 'R'; // Read-only
      if (dir_entry.attributes & 0x02)
        attr_str[1] = 'H'; // Hidden
      if (dir_entry.attributes & 0x04)
        attr_str[2] = 'S'; // System
      if (dir_entry.attributes & 0x08)
        attr_str[3] = 'V'; // Volume ID

      // Format output color based on type
      const char *entry_color = COLOR_WHITE;
      if (dir_entry.attributes & 0x10) {
        // Directory
        entry_color = COLOR_BLUE;
        attr_str[0] = 'D';
      } else if ((dir_entry.name[0] == '.') || (dir_entry.attributes & 0x08)) {
        // Special entry or volume label
        entry_color = COLOR_MAGENTA;
      }

      // Format date/time (simplified)
      char datetime[20] = "---";

      printf("%s%-12s %s%s%s %8u %19s %8u\n", entry_color, filename, BOLD,
             attr_str, COLOR_RESET, dir_entry.file_size, datetime,
             dir_entry.starting_cluster);

      entry_count++;
    }
  } else {
    // Non-root directory - not fully implemented in this mini version
    printf(
        "%sOnly root directory browsing implemented in this mini version%s\n",
        COLOR_YELLOW, COLOR_RESET);
  }

  if (entry_count == 0) {
    printf("%sDirectory is empty%s\n", COLOR_YELLOW, COLOR_RESET);
  }
  printf("\n");
}

// Display file content in hex/ascii dump
void display_file_content(FatState *state, uint32_t cluster, uint32_t size) {
  // Simplified implementation for the mini version
  printf(
      "%sFile content viewing not fully implemented in this mini version%s\n",
      COLOR_YELLOW, COLOR_RESET);
}

// Display help information
void print_help() {
  printf("\n%s%sAvailable Commands:%s\n", BOLD, COLOR_CYAN, COLOR_RESET);
  printf("  %sinfo%s        - Display filesystem information\n", COLOR_YELLOW,
         COLOR_RESET);
  printf("  %sls, dir%s     - List directory contents\n", COLOR_YELLOW,
         COLOR_RESET);
  printf("  %scd <dir>%s    - Change directory (not fully implemented)\n",
         COLOR_YELLOW, COLOR_RESET);
  printf("  %scat <file>%s  - View file content (not fully implemented)\n",
         COLOR_YELLOW, COLOR_RESET);
  printf("  %shelp%s        - Show this help message\n", COLOR_YELLOW,
         COLOR_RESET);
  printf("  %sexit, quit%s  - Exit the program\n", COLOR_YELLOW, COLOR_RESET);
  printf("\n");
}

// Clean up resources
void clean_up(FatState *state) {
  if (state->fp) {
    fclose(state->fp);
    state->fp = NULL;
  }
}
