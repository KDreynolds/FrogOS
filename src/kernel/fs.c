#include "kernel/fs.h"
#include "kernel/pmm.h"
#include "kernel/io.h"
#include "string.h"

#define MAX_PATH_LENGTH 256

static fs_entry_t fs_entries[MAX_FS_ENTRIES];
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

    print("FS: Initializing file system entries...\n");
    memset(fs_entries, 0, sizeof(fs_entries));
    free_block = 0;

    // Create root directory
    fs_entries[0].type = FS_DIRECTORY;
    fs_entries[0].parent = 0; // Root is its own parent
    strcpy(fs_entries[0].name, "/");
    fs_entries[0].size = 0;
    fs_entries[0].start_block = 0;
    fs_entries[0].is_used = true;

    print("FS: File system initialized\n");
}

static int find_free_entry(void) {
    for (int i = 1; i < MAX_FS_ENTRIES; i++) {
        if (!fs_entries[i].is_used) {
            return i;
        }
    }
    return -1;
}

int find_entry(const char* path) {
    print("Searching for path: ");
    print(path);
    print("\n");

    if (strcmp(path, "/") == 0) {
        print("Root directory found\n");
        return 0; // Root directory
    }

    char current_path[MAX_PATH_LENGTH];
    int current_dir = 0;

    const char* segment = path + 1; // Skip leading '/'
    const char* next_slash;

    while (*segment) {
        next_slash = strchr(segment, '/');
        int segment_len = next_slash ? (next_slash - segment) : strlen(segment);

        strncpy(current_path, segment, segment_len);
        current_path[segment_len] = '\0';

        bool found = false;
        for (int i = 0; i < MAX_FS_ENTRIES; i++) {
            if (fs_entries[i].is_used && fs_entries[i].parent == (uint32_t)current_dir &&
                strcmp(fs_entries[i].name, current_path) == 0) {
                if (!next_slash) {
                    return i; // Found the final entry
                }
                if (fs_entries[i].type != FS_DIRECTORY) {
                    return -1; // Not a directory
                }
                current_dir = i;
                found = true;
                break;
            }
        }

        if (!found) {
            return -1; // Path segment not found
        }

        segment = next_slash ? (next_slash + 1) : "";
    }

    return -1; // Should not reach here
}

int fs_create(const char* path, uint32_t size, fs_entry_type_t type) {
    print("Creating ");
    print(type == FS_DIRECTORY ? "directory" : "file");
    print(": ");
    print(path);
    print("\n");
    char parent_path[MAX_PATH_LENGTH];
    char name[MAX_FILENAME_LENGTH];

    // Extract parent path and name
    const char* last_slash = strrchr(path, '/');
    if (!last_slash) {
        print("Invalid path\n");
        return -1;
    }

    int parent_path_len = last_slash - path;
    strncpy(parent_path, path, parent_path_len);
    parent_path[parent_path_len] = '\0';
    if (parent_path_len == 0) {
        strcpy(parent_path, "/");
    }

    strcpy(name, last_slash + 1);

    int parent_index = find_entry(parent_path);
    print("Parent index: ");
    print_hex(parent_index);
    print("\n");

    if (parent_index == -1 || (parent_index != 0 && fs_entries[parent_index].type != FS_DIRECTORY)) {
        print("Parent directory not found\n");
        return -1;
    }

    int entry = find_free_entry();
    if (entry == -1) {
        print("No free file system entries\n");
        return -1;
    }

    if (type == FS_FILE) {
        uint32_t blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        if (free_block + blocks_needed > FS_SIZE / BLOCK_SIZE) {
            print("Not enough space\n");
            return -1;
        }
        fs_entries[entry].start_block = free_block;
        free_block += blocks_needed;
    } else {
        fs_entries[entry].start_block = 0;
    }

    strcpy(fs_entries[entry].name, name);
    fs_entries[entry].parent = parent_index;
    fs_entries[entry].size = size;
    fs_entries[entry].type = type;
    fs_entries[entry].is_used = true;

    return 0;
}

int fs_delete(const char* path) {
    int entry_index = find_entry(path);
    if (entry_index == -1) {
        print("Entry not found\n");
        return -1;
    }

    if (fs_entries[entry_index].type == FS_DIRECTORY) {
        // Check if directory is empty
        for (int i = 0; i < MAX_FS_ENTRIES; i++) {
            if (fs_entries[i].is_used && fs_entries[i].parent == (uint32_t)entry_index) {
                print("Directory not empty\n");
                return -1;
            }
        }
    }

    fs_entries[entry_index].is_used = false;
    return 0;
}

int fs_read(const char* path, void* buffer, uint32_t size, uint32_t offset) {
    int file_index = find_entry(path);
    if (file_index == -1 || fs_entries[file_index].type != FS_FILE) {
        print("File not found\n");
        return -1;
    }

    if (offset + size > fs_entries[file_index].size) {
        print("Read out of bounds\n");
        return -1;
    }

    uint32_t start = fs_entries[file_index].start_block * BLOCK_SIZE + offset;
    memcpy(buffer, fs_data + start, size);
    return size;
}

int fs_write(const char* path, const void* buffer, uint32_t size, uint32_t offset) {
    int file_index = find_entry(path);
    if (file_index == -1 || fs_entries[file_index].type != FS_FILE) {
        print("File not found\n");
        return -1;
    }

    if (offset + size > fs_entries[file_index].size) {
        print("Write out of bounds\n");
        return -1;
    }

    uint32_t start = fs_entries[file_index].start_block * BLOCK_SIZE + offset;
    memcpy(fs_data + start, buffer, size);
    return size;
}

void fs_list(const char* path) {
    int dir_index = find_entry(path);
    if (dir_index == -1 || fs_entries[dir_index].type != FS_DIRECTORY) {
        print("Directory not found\n");
        return;
    }

    print("Contents of ");
    print(path);
    print(":\n");

    for (int i = 0; i < MAX_FS_ENTRIES; i++) {
        if (fs_entries[i].is_used && fs_entries[i].parent == (uint32_t)dir_index) {
            print(fs_entries[i].type == FS_DIRECTORY ? "[DIR] " : "[FILE] ");
            print(fs_entries[i].name);
            if (fs_entries[i].type == FS_FILE) {
                print(" (");
                print_hex(fs_entries[i].size);
                print(" bytes)");
            }
            print("\n");
        }
    }
}