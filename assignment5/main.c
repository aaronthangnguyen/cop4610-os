/* ASSIGNMENT 5
 * Name: Aaron Nguyen
 * PID: 6403474
 */

/*
 * FIFO and LRU Page Replacement Algorithms Implementation
 *
 * This program simulates memory management using FIFO (First-In-First-Out) and LRU (Least Recently Used)
 * page replacement strategies, crucial for understanding how operating systems handle page faults. It reads
 * page references from a file and the available memory frames from command line arguments, then processes
 * these references to calculate and display the number of page faults incurred by each algorithm, alongside
 * the final memory state.
 *
 * Key Features:
 * - Command line argument processing for dynamic input of page references and memory frame count.
 * - FIFO algorithm implementation for managing page replacements by queuing pages in memory.
 * - LRU algorithm implementation for prioritizing page replacements based on recency of use.
 * - Dynamic handling of an arbitrary sequence of page references from input files.
 * - Comparative output of page faults and final memory state for both FIFO and LRU.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAGES 1000
//
// Find the index of a page in the memory, or return -1 if not found.
int find_page_index(int page, int memory[], int num_of_frames) {
    for (int i = 0; i < num_of_frames; i++) {
        if (memory[i] == page) {
            return i;
        }
    }
    return -1;
}

// FIFO Algorithm
int fifo(int pages[], int num_of_pages, int num_of_frames, int final_memory[]) {
    int memory[num_of_frames], page_faults = 0, pos = 0;
    memset(memory, -1, sizeof(memory)); // Initialize memory with -1

    for (int i = 0; i < num_of_pages; i++) {
        if (find_page_index(pages[i], memory, num_of_frames) == -1) {
            memory[pos] = pages[i];
            pos = (pos + 1) % num_of_frames;
            page_faults++;
        }
    }

    // Copy the final state of memory
    memcpy(final_memory, memory, sizeof(int) * num_of_frames);

    return page_faults;
}


// LRU Algorithm Implementation
int lru(int pages[], int num_of_pages, int num_of_frames, int final_memory[]) {
    int memory[num_of_frames], page_faults = 0;
    int last_used[num_of_frames]; // Tracks the last use time of pages in memory
    memset(memory, -1, sizeof(memory)); // Initialize memory
    memset(last_used, 0, sizeof(last_used)); // Initialize last used times

    for (int i = 0; i < num_of_pages; i++) {
        int found = find_page_index(pages[i], memory, num_of_frames);
        if (found != -1) {
            // Page is already in memory, update its last used time
            last_used[found] = i;
        } else {
            // Page fault, find the LRU page to replace
            int lru = 0, min_last_used = i;
            for (int j = 0; j < num_of_frames; j++) {
                if (memory[j] == -1) { // If there's free space in memory
                    lru = j;
                    break;
                } else if (last_used[j] < min_last_used) {
                    min_last_used = last_used[j];
                    lru = j;
                }
            }
            // Replace the LRU page
            memory[lru] = pages[i];
            last_used[lru] = i;
            page_faults++;
        }
    }

    // Copy the final state of memory
    memcpy(final_memory, memory, sizeof(int) * num_of_frames);

    return page_faults;
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s page_ref_file num_of_frames\n", argv[0]);
        return 1;
    }

    int num_of_frames = atoi(argv[2]);
    if (num_of_frames < 1 || num_of_frames > 10) {
        printf("num_of_frames must be between 1 and 10\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    int pages[MAX_PAGES], num_of_pages = 0;
    while (fscanf(file, "%d", &pages[num_of_pages]) != EOF) {
        num_of_pages++;
    }
    fclose(file);

    int final_memory_fifo[num_of_frames], final_memory_lru[num_of_frames];
    int fifo_page_faults = fifo(pages, num_of_pages, num_of_frames, final_memory_fifo);
    int lru_page_faults = lru(pages, num_of_pages, num_of_frames, final_memory_lru);

    printf("FIFO: %d page faults\n", fifo_page_faults);
    printf("Final state of memory: ");
    for (int i = 0; i < num_of_frames; i++) {
        if (final_memory_fifo[i] != -1) printf("%d ", final_memory_fifo[i]);
    }
    printf("\nLRU: %d page faults\n", lru_page_faults);
    printf("Final state of memory: ");
    for (int i = 0; i < num_of_frames; i++) {
        if (final_memory_lru[i] != -1) printf("%d ", final_memory_lru[i]);
    }
    printf("\n");

    return 0;
}
