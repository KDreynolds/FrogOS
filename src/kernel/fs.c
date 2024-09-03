#include "kernel/fs.h"
#include "kernel/pmm.h"
#include "kernel/io.h"
#include "string.h"

static file_entry_t file_table[MAX_FILES];
static uint8_t* fs_data;
static uint32_t free_block;

void fs_init(void) {
    print("FS: Allocating memory for file system...\n");
    fs_data = pmm_alloc_page();
    if (!fs_data) {
        print("FS: Failed to allocate memory for file system\n");
        return;
    }
    print("FS: Memory allocated successfully\n");

    print("FS: Initializing file table...\n");
    memset(file_table, 0, sizeof(file_table));
    free_block = 0;
    print("FS: File table initialized\n");

    print("FS: Initialization complete\n");
}

static int find_free_entry(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_table[i].is_used) {
            return i;
        }
    }
    return -1;
}

static int find_file(const char* filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].is_used && strcmp(file_table[i].filename, filename) == 0) {
            return i;
        }
    }
    return -1;
}

int fs_create(const char* filename, uint32_t size) {
    if (find_file(filename) != -1) {
        print("File already exists\n");
        return -1;
    }

    int entry = find_free_entry();
    if (entry == -1) {
        print("No free file entries\n");
        return -1;
    }

    uint32_t blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (free_block + blocks_needed > FS_SIZE / BLOCK_SIZE) {
        print("Not enough space\n");
        return -1;
    }

    strncpy(file_table[entry].filename, filename, MAX_FILENAME_LENGTH - 1);
    file_table[entry].filename[MAX_FILENAME_LENGTH - 1] = '\0';
    file_table[entry].start_block = free_block;
    file_table[entry].size = size;
    file_table[entry].is_used = true;

    free_block += blocks_needed;
    return 0;
}

int fs_delete(const char* filename) {
    int file_index = find_file(filename);
    if (file_index == -1) {
        print("File not found\n");
        return -1;
    }

    file_table[file_index].is_used = false;
    return 0;
}

int fs_read(const char* filename, void* buffer, uint32_t size, uint32_t offset) {
    int file_index = find_file(filename);
    if (file_index == -1) {
        print("File not found\n");
        return -1;
    }

    if (offset + size > file_table[file_index].size) {
        print("Read out of bounds\n");
        return -1;
    }

    uint32_t start = file_table[file_index].start_block * BLOCK_SIZE + offset;
    memcpy(buffer, fs_data + start, size);
    return size;
}

int fs_write(const char* filename, const void* buffer, uint32_t size, uint32_t offset) {
    int file_index = find_file(filename);
    if (file_index == -1) {
        print("File not found\n");
        return -1;
    }

    if (offset + size > file_table[file_index].size) {
        print("Write out of bounds\n");
        return -1;
    }

    uint32_t start = file_table[file_index].start_block * BLOCK_SIZE + offset;
    memcpy(fs_data + start, buffer, size);
    return size;
}

void fs_list(void) {
    print("File list:\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].is_used) {
            print(file_table[i].filename);
            print(" (");
            print_hex(file_table[i].size);
            print(" bytes)\n");
        }
    }
}