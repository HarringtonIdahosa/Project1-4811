#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Function to generate a random number between min and max (inclusive)
int getRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int main() {
    int num_children;
    char directory[50];

    printf("Copy and paste the directory of the test cases: ");
    scanf("%s", directory);

    printf("How many child processes do you want (max 25)? ");
    scanf("%d", &num_children);

    if (num_children > 25) {
        printf("Over capacity\n");
        return 0;
    }

    printf("Parent PID is %d\n", getpid());

    for (int child_number = 1; child_number <= num_children; ++child_number) {
        pid_t pid = fork(); // Create a child process

        if (pid < 0) {
            // Error occurred
            fprintf(stderr, "Fork failed\n");
            return 1;
        } else if (pid == 0) {
            // Child process
            printf("Started child %d with PID = %d\n", child_number, getpid());

            // Determine the test case based on the child process index
            int test_case = child_number % 5 + 1;

            // command string based on the selected test case and user-provided directory
            char command[100];
            snprintf(command, sizeof(command), "%s/test%d", directory, test_case);

            // Execute the selected test case
            if (execlp(command, command, NULL) == -1) {
                // If execlp fails
                perror("Error in execlp");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Parent process - wait for all child processes to complete
    for (int i = 0; i < num_children; ++i) {
        int status;
        pid_t child_pid = wait(&status);

        // Child has finished
        printf("Child %d (PID %d) finished\n", i + 1, child_pid);
    }

    return 0;
}
