#include "kernel/shell.h"
#include "kernel/uart.h"
#include "kernel/io.h"
#include "kernel/pmm.h"
#include "kernel/fs.h"
#include <stddef.h>
#include <stdint.h>
#include "string.h" 

#define MAX_COMMAND_LENGTH 256
#define MAX_PATH_LENGTH 256

// Function prototypes
static void shell_execute_command(const char* command);
static int parse_args(const char* command, char* cmd, char* arg1, char* arg2);
static int str_to_int(const char* str);

// New function prototypes
static void cmd_mkdir(const char* path);
static void cmd_cd(const char* path);
static void cmd_pwd(void);
static void cmd_ls(const char* path);

// Current working directory
static char current_directory[MAX_PATH_LENGTH] = "/";

void shell_run() {
    print("Shell: Entering shell loop\n");
    char command[MAX_COMMAND_LENGTH];
    size_t command_length = 0;

    while (1) {
        print(current_directory);
        print("$ ");
        
        // Read command
        command_length = 0;
        while (1) {
            char c = uart_getc();
            if (c == '\r' || c == '\n') {
                uart_putc('\n');
                command[command_length] = '\0';
                break;
            } else if (c == '\b' && command_length > 0) {
                uart_puts("\b \b");
                command_length--;
            } else if (command_length < MAX_COMMAND_LENGTH - 1) {
                uart_putc(c);
                command[command_length++] = c;
            }
        }

        // Execute command
        shell_execute_command(command);
    }
}

static void shell_execute_command(const char* command) {
    char cmd[MAX_COMMAND_LENGTH];
    char arg1[MAX_COMMAND_LENGTH];
    char arg2[MAX_COMMAND_LENGTH];
    int args = parse_args(command, cmd, arg1, arg2);

    if (args == 0) {
        print("Empty command\n");
        return;
    }

    if (strcmp(cmd, "help") == 0) {
        print("Available commands:\n");
        print("  help - Display this help message\n");
        print("  hello - Print a greeting\n");
        print("  memory - Display memory information\n");
        print("  fs_create <filename> <size> - Create a new file\n");
        print("  fs_delete <filename> - Delete a file\n");
        print("  ls [path] - List contents of a directory\n");
        print("  mkdir <path> - Create a new directory\n");
        print("  cd <path> - Change current directory\n");
        print("  pwd - Print current working directory\n");
    } else if (strcmp(cmd, "hello") == 0) {
        print("Hello from MyOS!\n");
    } else if (strcmp(cmd, "memory") == 0) {
        uint64_t free_mem = pmm_get_free_memory();
        print("Free memory: ");
        print_hex(free_mem);
        print(" bytes\n");
    } else if (strcmp(cmd, "fs_create") == 0 && args == 3) {
        uint32_t size = str_to_int(arg2);
        if (fs_create(arg1, size, FS_FILE) == 0) {
            print("File created successfully\n");
        }
    } else if (strcmp(cmd, "fs_delete") == 0 && args == 2) {
        if (fs_delete(arg1) == 0) {
            print("File deleted successfully\n");
        }
    } else if (strcmp(cmd, "ls") == 0) {
        cmd_ls(args == 2 ? arg1 : current_directory);
    } else if (strcmp(cmd, "mkdir") == 0 && args == 2) {
        cmd_mkdir(arg1);
    } else if (strcmp(cmd, "cd") == 0 && args == 2) {
        cmd_cd(arg1);
    } else if (strcmp(cmd, "pwd") == 0) {
        cmd_pwd();
    } else {
        print("Unknown command. Type 'help' for available commands.\n");
    }
}

static void cmd_mkdir(const char* path) {
    char full_path[MAX_PATH_LENGTH];
    if (path[0] == '/') {
        strcpy(full_path, path);
    } else {
        strcpy(full_path, current_directory);
        if (strcmp(current_directory, "/") != 0) {
            strcat(full_path, "/");
        }
        strcat(full_path, path);
    }

    if (fs_create(full_path, 0, FS_DIRECTORY) == 0) {
        print("Directory created successfully\n");
    } else {
        print("Failed to create directory\n");
    }
}

static void cmd_cd(const char* path) {
    char new_path[MAX_PATH_LENGTH];
    if (path[0] == '/') {
        strcpy(new_path, path);
    } else {
        strcpy(new_path, current_directory);
        if (strcmp(current_directory, "/") != 0) {
            strcat(new_path, "/");
        }
        strcat(new_path, path);
    }

    int entry = find_entry(new_path);
    if (entry != -1 && find_entry(new_path) != -1) {
        strcpy(current_directory, new_path);
    } else {
        print("Invalid directory\n");
    }
}

static void cmd_pwd(void) {
    print(current_directory);
    print("\n");
}

static void cmd_ls(const char* path) {
    fs_list(path);
}

static int parse_args(const char* command, char* cmd, char* arg1, char* arg2) {
    int args = 0;
    const char* start = command;
    const char* end = command;

    while (*end && *end != ' ') end++;
    if (end > start) {
        int len = end - start;
        strncpy(cmd, start, len);
        cmd[len] = '\0';
        args++;
    }

    if (*end) {
        start = end + 1;
        while (*start == ' ') start++;
        end = start;
        while (*end && *end != ' ') end++;
        if (end > start) {
            int len = end - start;
            strncpy(arg1, start, len);
            arg1[len] = '\0';
            args++;
        }
    }

    if (*end) {
        start = end + 1;
        while (*start == ' ') start++;
        end = start;
        while (*end && *end != ' ') end++;
        if (end > start) {
            int len = end - start;
            strncpy(arg2, start, len);
            arg2[len] = '\0';
            args++;
        }
    }

    return args;
}

static int str_to_int(const char* str) {
    int result = 0;
    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    }
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return sign * result;
}