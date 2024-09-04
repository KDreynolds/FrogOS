#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_FILENAME_LENGTH 32
#define MAX_FS_ENTRIES 256
#define BLOCK_SIZE 512
#define FS_SIZE (1024 * 1024) // 1 MB file system

typedef enum {
    FS_FILE,
    FS_DIRECTORY
} fs_entry_type_t;

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    fs_entry_type_t type;
    uint32_t parent;
    uint32_t start_block;
    uint32_t size;
    bool is_used;
} fs_entry_t;

void fs_init(void);
int fs_create(const char* path, uint32_t size, fs_entry_type_t type);
int fs_delete(const char* path);
int fs_read(const char* path, void* buffer, uint32_t size, uint32_t offset);
int fs_write(const char* path, const void* buffer, uint32_t size, uint32_t offset);
void fs_list(const char* path);
int find_entry(const char* path);

#endif // FS_H