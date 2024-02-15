#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

// Function to generate a random number between min and max (inclusive)
int getRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_children>", argv[0]);
        return 1;
    }

    int num_children = atoi(argv[1]);

    if (num_children <= 0 || num_children > 25) {
        fprintf(stderr, "Max 25.\n");
        return 1;
    }

    char executable_path[1024]; 

    // Try to get the full path of the executable
    if (realpath(argv[0], executable_path) == NULL) {
        perror("Error getting executable path");
        return 1;
    }

    // Extract the directory 
    char *executable_dir = dirname(strdup(executable_path));


    printf("Parent PID is %d\n", getpid());

    for (int child_number = 1; child_number <= num_children; ++child_number) {
        pid_t pid = fork(); // Create a child process

        if (pid < 0) {
            // Error occurred
            fprintf(stderr, "Fork failed\n");
            return 1;
        } else if (pid == 0) {
            // Child process

            // Change the current working directory back to the original directory
            if (chdir(executable_dir) == -1) {
                perror("Error changing directory back to original");
                exit(EXIT_FAILURE);
            }

            printf("Started child %d with PID = %d\n", child_number, getpid());

            // Determine the test case based on the child process index
            int test_case = child_number % 5 + 1;

            // command string based on the selected test case and executable directory
            char command[1024]; 
            snprintf(command, sizeof(command), "%s/test%d", executable_dir, test_case);

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

    free(executable_dir); // Free memory allocated by strdup

    return 0;
}
