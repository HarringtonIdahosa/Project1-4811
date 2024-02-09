#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int num_children;

    printf("How many child processes do you want (max 25)? ");
    scanf("%d", &num_children);

    if (num_children > 25) {
        printf("Over capacity\n");
        return 0;
    }

    printf("Parent PID is %d\n", getpid());
    int child_pids[num_children];

    for (int child_number = 1; child_number <= num_children; ++child_number) {
        pid_t pid = fork(); // Create a child process

        if (pid < 0) {
            // Error occurred
            fprintf(stderr, "Fork failed\n");
            return 1;
        } else if (pid == 0) {
            // Child process
            printf("Started child %d with PID = %d\n", child_number, getpid());

            execlp("/bin/ls", "ls", NULL);

            // If execlp fails
            fprintf(stderr, "Error in execlp\n");
            return 1;
        } else {
            // Save child PID in the array
            child_pids[child_number - 1] = pid;
        }
    }

    // Parent process - wait for all child processes to complete
    for (int i = 0; i < num_children; ++i) {
        int status;
        pid_t child_pid = wait(&status);

        for (int j = 0; j < num_children; ++j) {
            if (child_pid == child_pids[j]) {
                // Child has finished
                printf("Child %d (PID %d) finished\n", j + 1, child_pid);
                break;
            }
        }
    }

    return 0;
}

