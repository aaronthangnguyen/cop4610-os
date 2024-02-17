/*
ASSIGNMENT 1
Name: Aaron Nguyen
PID: 6403474
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// Function to calculate the factorial of a number
int task_factorial(int n)
{
    if (n < 0)
    {
        printf("Factorial of %d is undefined\n", n);
        return -1;
    }
    int res = 1;
    for (int i = 1; i <= n; i++)
        res *= i;

    sleep(1); // Sleep to simulate a time-consuming task
    return res;
}

// Function to compute the nth Fibonacci number
int task_fibonacci(int n)
{
    int prev2 = 0;
    int prev1 = 1;
    for (int i = 1; i < n; i++)
    {
        int tmp = prev1;
        prev1 += prev2;
        prev2 = tmp;
    }

    sleep(2); // Sleep to simulate a time-consuming task
    return prev1;
}

// Function to find prime numbers up to a specified number n
int *task_primes(int n)
{
    if (n > 100)
    {
        printf("n exceeds the maximum limit of %d.\n", 100);
        return NULL;
    }

    // Dynamic allocation to store prime numbers
    int *arr = (int *)malloc(n * sizeof(int));
    if (!arr)
    {
        perror("Allocation failed.");
        exit(EXIT_FAILURE);
    }

    int count = 0; // Count of prime numbers

    for (int i = 2; i <= n; i++)
    {
        int is_prime = 1;
        for (int j = 2; j * j <= i; j++)
        {
            if (i % j == 0)
            {
                is_prime = 0;
                break;
            }
        }
        if (is_prime)
        {
            arr[count] = i;
            count++;
        }
    }

    if (count < n)
        arr[count] = -1; // Mark the end of prime numbers in the array

    sleep(5); // Sleep to simulate a time-consuming task
    return arr;
}

// Function to run specific tasks based on index
void run_task(int i, int pid)
{
    int result;
    int task_no = i + 1;
    int *arr = NULL; // Pointer for dynamically allocated array
    printf("Child %d (PID: %d) is ", task_no, pid);

    // Switch case to handle different tasks
    switch (i)
    {
    case 0:
        // Task 0: Factorial of 5
        printf("computing the factorial of 5.\n");
        result = task_factorial(5);
        printf("Child %d (PID: %d) completed its task. Result: %d\n", task_no, pid, result);
        break;
    case 1:
        // Task 1: Prime numbers of 20
        printf("finding prime numbers up to 20.\n");
        arr = task_primes(20);
        printf("Child %d (PID: %d) completed its task. Result: ", task_no, pid);
        for (int i = 0; i < 20 && arr[i] != -1; i++)
            printf("%d ", arr[i]);
        printf("\n");
        break;
    case 2:
        // Task 2: Fibonacci of 7
        printf("computing the fibonacci of 7.\n");
        result = task_fibonacci(7);
        printf("Child %d (PID: %d) completed its task. Result: %d\n", task_no, pid, result);
        break;
    case 3:
        // Task 3: Fibonacci of 28
        printf("computing the fibonacci of 28.\n");
        result = task_fibonacci(28);
        printf("Child %d (PID: %d) completed its task. Result: %d\n", task_no, pid, result);
        break;
    case 4:
        // Task 4: Prime numbers of 46
        printf("finding prime numbers up to 46.\n");
        arr = task_primes(46);
        printf("Child %d (PID: %d) completed its task. Result: ", task_no, pid);
        for (int i = 0; i < 46 && arr[i] != -1; i++)
            printf("%d ", arr[i]);
        printf("\n");
        break;
    default:
        // Default Case: Custom task
        printf("performing a custom task.\n");
        printf("Child %d (PID: %d) completed its task. Result: Custom task completed.\n", task_no, pid);
        break;
    }

    if (arr != NULL)
        free(arr); // Free dynamically allocated memory
}

// Function to read an integer input within range
int int_input(const char *prompt, int min, int max)
{
    int value;
    do
    {
        printf("%s", prompt);
        scanf("%d", &value);
    } while (value < min || value > max);
    return value;
}

// Main function
int main(void)
{
    int num_processes = int_input("Enter the number of child processes to create: ", 1, 5);

    pid_t parent_pid = getppid();
    printf("Parent process (%d) is creating %d child processes.\n\n", parent_pid, num_processes);

    pid_t *children_pid = malloc(num_processes * sizeof(pid_t));
    // Create processes
    for (int i = 0; i < num_processes; ++i)
    {
        children_pid[i] = fork();
        if (children_pid[i] == -1)
        {
            perror("Fork failed.");
            exit(EXIT_FAILURE);
        }
        else if (children_pid[i] == 0)
        {
            run_task(i, getpid());
            exit(0);
        }
    }

    // Waiting for all child processes to complete
    for (int i = 0; i < num_processes; ++i)
    {
        int status;
        int task_no = i + 1;
        pid_t cpid = waitpid(children_pid[i], &status, 0);

        if (cpid == -1) // Handle waitpid() error
        {
            char error_message[256];
            snprintf(error_message, sizeof(error_message), "Wait for child %d (PID: %d) failed.", task_no, cpid);
            perror(error_message);
            continue;
        }
        else if (WIFSIGNALED(status)) // Child terminated by signal
            printf("Child %d (PID: %d) terminated with status %d.\n", task_no, cpid, WEXITSTATUS(status));
    }

    free(children_pid);

    printf("\nAll child processes have completed. Parent (PID: %d) is displaying the final message.\n", parent_pid);

    return 0;
}
