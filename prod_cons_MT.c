// prod_cons_MT.c

#include "prod_cons_MT.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_full = PTHREAD_COND_INITIALIZER;

void init_monitor(Monitor* monitor) {
    monitor->in = 0;
    monitor->out = 0;
}

void produce(Monitor* monitor, int producer_id, int num_values) {
    for (int i = 0; i < num_values; ++i) {
        // random value between 1 and 10
        int value = rand() % 10 + 1;

        pthread_mutex_lock(&mutex);

        // Check if the buffer is full
        while (((monitor->in + 1) % BUFFER_SIZE) == monitor->out) {
            printf("P%d: Blocked due to full buffer\n", producer_id);

            // Save the current in index to detect changes
            int initial_in = monitor->in;

            pthread_cond_wait(&buffer_full, &mutex);

            // if the in index has changed, indicate a change in the buffer state
            if (initial_in == monitor->in) {
                printf("P%d: Done waiting on full buffer\n", producer_id);
            }
        }

        // Write to the buffer
        monitor->buffer[monitor->in] = value;

        // Print the produced value and position
        printf("P%d: Writing %d to position %d\n", producer_id, value, monitor->in);

        // Update the 'in' index
        monitor->in = (monitor->in + 1) % BUFFER_SIZE;

        pthread_cond_signal(&buffer_empty);
        pthread_mutex_unlock(&mutex);
    }
}

void consume(Monitor* monitor, int consumer_id, int num_values) {
    for (int i = 0; i < num_values; ++i) {
        int was_full = 0;  // Flag to indicate if the buffer was full when starting to consume

        pthread_mutex_lock(&mutex);

        // Check if the buffer is empty
        while (monitor->in == monitor->out) {
            printf("C%d: Blocked due to empty buffer\n", consumer_id);

            // Save the current out index to detect changes
            int initial_out = monitor->out;

            // Set the flag if the buffer was full when starting to consume
            was_full = ((monitor->in + 1) % BUFFER_SIZE) == initial_out;

            pthread_cond_wait(&buffer_empty, &mutex);

            // Check if the out index has changed, indicating a change in the buffer state
            if (initial_out == monitor->out) {
                printf("C%d: Done waiting on empty buffer\n", consumer_id);
            }
        }

        // Read from the buffer
        int value = monitor->buffer[monitor->out];

        // Print the consumed value and position
        printf("C%d: Reading %d from position %d\n", consumer_id, value, monitor->out);

        // Update the 'out' index
        monitor->out = (monitor->out + 1) % BUFFER_SIZE;

        pthread_cond_signal(&buffer_full);
        pthread_mutex_unlock(&mutex);

        // Print a message when done working on a full buffer
        if (was_full && i % 3 == 0) {
            printf("C%d: Done working on full buffer\n", consumer_id);
        }
    }
}

void destroy_monitor(Monitor* monitor) {
   
}
