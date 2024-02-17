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

    printf("Order of Execution: ");
    for (int i = 0; i < num_process; i++)
    {
        if (current_time < arrival[i])
            current_time = arrival[i];

        int waiting = current_time - arrival[i];
        int turnaround = burst[i] + waiting;

        total_waiting += waiting;
        total_turnaround += turnaround;
        current_time += burst[i];

        printf("P%d", i + 1);
        if (i < num_process - 1)
            printf(" -> ");
    }
    printf("\n");

    printf("Average Waiting Time: %.2f\n", (float)total_waiting / num_process);
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

    printf("Order of Execution: ");
    for (int i = 0; i < num_process; i++)
    {
        int min_index = i;
        for (int j = i + 1; j < num_process; j++)
        {
            if (arrival[j] <= current_time && burst[j] < burst[min_index])
                min_index = j;
        }

        swap(&arrival[i], &arrival[min_index]);
        swap(&burst[i], &burst[min_index]);
        swap(&process_id[i], &process_id[min_index]);

        if (current_time < arrival[i])
            current_time = arrival[i];

        int waiting = current_time - arrival[i];
        int turnaround = waiting + burst[i];

        total_waiting += waiting;
        total_turnaround += turnaround;
        current_time += burst[i];

        printf("P%d", process_id[i]);
        if (i < num_process - 1)
            printf(" -> ");
    }

    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting / num_process);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround / num_process);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <filename> <algo>\n", argv[0]);
        return 1;
    }

    if (!is_valid_algo(argv[2]))
    {
        fprintf(stderr, "Error: '%s' is not a valid scheduling algorithm.\nValid options are 'FCFS' or 'SJF'.\n", argv[2]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Cannot open '%s'.\n", argv[1]);
        return 1;
    }

    int num_process;
    fscanf(file, "%d", &num_process);

    int *arrival = (int *)malloc(num_process * sizeof(int));
    int *burst = (int *)malloc(num_process * sizeof(int));

    for (int i = 0; i < num_process; i++)
        fscanf(file, "%d %d", &arrival[i], &burst[i]);

    fclose(file); // Close file after reading

    // Select scheduling algorithm based on input
    if (strcmp(argv[2], "FCFS") == 0)
        fcfs(num_process, arrival, burst);
    else if (strcmp(argv[2], "SJF") == 0)
        sjf(num_process, arrival, burst);

    // Release the dynamically allocated memory
    free(arrival);
    free(burst);

    return 0;
}
