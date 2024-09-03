#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_FILENAME_LENGTH 32
#define MAX_FILES 64
#define BLOCK_SIZE 512
#define FS_SIZE (1024 * 1024) // 1 MB file system

typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    uint32_t start_block;
    uint32_t size;
    bool is_used;
} file_entry_t;

void fs_init(void);
int fs_create(const char* filename, uint32_t size);
int fs_delete(const char* filename);
int fs_read(const char* filename, void* buffer, uint32_t size, uint32_t offset);
int fs_write(const char* filename, const void* buffer, uint32_t size, uint32_t offset);
void fs_list(void);

#endif // FS_H