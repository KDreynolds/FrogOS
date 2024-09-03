#include "kernel/shell.h"
#include "kernel/uart.h"
#include "kernel/io.h"
#include "kernel/pmm.h"
#include "kernel/fs.h"
#include <stddef.h>
#include <stdint.h>
#include "string.h"

#define MAX_COMMAND_LENGTH 256

// Function prototypes
static void shell_execute_command(const char* command);
static int parse_args(const char* command, char* cmd, char* arg1, char* arg2);
static int str_to_int(const char* str);

void shell_run() {
    print("Shell: Entering shell loop\n");
    char command[MAX_COMMAND_LENGTH];
    size_t command_length = 0;

    while (1) {
        print("MyOS> ");
        
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

        print("Shell: Executing command: ");
        print(command);
        print("\n");

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
        print("  fs_list - List all files\n");
    } else if (strcmp(cmd, "hello") == 0) {
        print("Hello from MyOS!\n");
    } else if (strcmp(cmd, "memory") == 0) {
        uint64_t free_mem = pmm_get_free_memory();
        print("Free memory: ");
        print_hex(free_mem);
        print(" bytes\n");
    } else if (strcmp(cmd, "fs_create") == 0 && args == 3) {
        uint32_t size = str_to_int(arg2);
        if (fs_create(arg1, size) == 0) {
            print("File created successfully\n");
        }
    } else if (strcmp(cmd, "fs_delete") == 0 && args == 2) {
        if (fs_delete(arg1) == 0) {
            print("File deleted successfully\n");
        }
    } else if (strcmp(cmd, "fs_list") == 0) {
        fs_list();
    } else {
        print("Unknown command. Type 'help' for available commands.\n");
    }
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