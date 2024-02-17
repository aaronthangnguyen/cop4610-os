/* ASSIGNMENT 2
 * Name: Aaron Nguyen
 * PID: 6403474
 */

/* Scheduling algorithm simulator:
 * - Reads process data from file
 * - Executes specified algorithm
 * - Displays results: execution order, average waiting time, and turnaround time
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Validates scheduling algorithm
int is_valid_algo(char *algo)
{
    return strcmp(algo, "FCFS") == 0 || strcmp(algo, "SJF") == 0;
}

// First-Come, First-Served scheduling
void fcfs(int num_process, int *arrival, int *burst)
{
    int total_turnaround = 0, total_waiting = 0, current_time = 0;

    // Execute FCFS algorithm
    printf("Order of Execution: ");
    for (int i = 0; i < num_process; i++)
    {
        // Ensure execution order and calculate waiting and turnaround times
        if (current_time < arrival[i])
            current_time = arrival[i];

        int waiting = current_time - arrival[i];
        int turnaround = burst[i] + waiting;

        total_waiting += waiting;
        total_turnaround += turnaround;
        current_time += burst[i];

        // Display execution order
        printf("P%d", i + 1);
        if (i < num_process - 1)
            printf(" -> ");
    }

    // Display average waiting and turnaround times
    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting / num_process);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround / num_process);
}

// Swaps two integers
void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Shortest Job First scheduling
void sjf(int num_process, int *arrival, int *burst)
{
    int total_waiting = 0, total_turnaround = 0, current_time = 0;
    int process_id[num_process]; // Tracks process IDs

    for (int i = 0; i < num_process; i++)
        process_id[i] = i + 1;

    // Execute SJF algorithm
    printf("Order of Execution: ");
    for (int i = 0; i < num_process; i++)
    {
        int min_index = i;
        for (int j = i + 1; j < num_process; j++)
        {
            // Find process with the shortest burst time
            if (arrival[j] <= current_time && burst[j] < burst[min_index])
                min_index = j;
        }

        // Swap process information for SJF
        swap(&arrival[i], &arrival[min_index]);
        swap(&burst[i], &burst[min_index]);
        swap(&process_id[i], &process_id[min_index]);

        // Adjust current time and calculate waiting and turnaround times
        if (current_time < arrival[i])
            current_time = arrival[i];

        int waiting = current_time - arrival[i];
        int turnaround = waiting + burst[i];

        total_waiting += waiting;
        total_turnaround += turnaround;
        current_time += burst[i];

        // Display execution order
        printf("P%d", process_id[i]);
        if (i < num_process - 1)
            printf(" -> ");
    }

    // Display average waiting and turnaround times
    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting / num_process);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround / num_process);
}

int main(int argc, char *argv[])
{
    // Ensure correct command-line arguments
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <filename> <algo>\n", argv[0]);
        return 1;
    }

    // Validate scheduling algorithm
    if (!is_valid_algo(argv[2]))
    {
        fprintf(stderr, "Error: '%s' is not a valid scheduling algorithm.\nValid options are 'FCFS' or 'SJF'.\n", argv[2]);
        return 1;
    }

    // Open file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Cannot open '%s'.\n", argv[1]);
        return 1;
    }

    // Read number of processes
    int num_process;
    fscanf(file, "%d", &num_process);

    // Allocate memory for arrival and burst arrays
    int *arrival = (int *)malloc(num_process * sizeof(int));
    int *burst = (int *)malloc(num_process * sizeof(int));

    // Read arrival and burst times from file
    for (int i = 0; i < num_process; i++)
        fscanf(file, "%d %d", &arrival[i], &burst[i]);

    // Close file after reading
    fclose(file);

    // Select scheduling algorithm based on input
    if (strcmp(argv[2], "FCFS") == 0)
        fcfs(num_process, arrival, burst);
    else if (strcmp(argv[2], "SJF") == 0)
        sjf(num_process, arrival, burst);

    // Free dynamically allocated memory
    free(arrival);
    free(burst);

    return 0;
}
