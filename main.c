// Main.c

#include "prod_cons_MT.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Monitor monitor;

void* producer(void* arg) {
    int* params = (int*)arg;
    int producer_id = params[0];
    int total_producer_values = params[1];

    printf("Main: started producer %d\n", producer_id);
    printf("P%d: Started, producing %d values\n", producer_id, total_producer_values);

    produce(&monitor, producer_id, total_producer_values);

    printf("P%d: Exiting\n", producer_id);

    pthread_exit(NULL);
}

void* consumer(void* arg) {
    int* params = (int*)arg;
    int consumer_id = params[0];
    int total_producer_values = params[1];
    int num_consumers = params[2];

    printf("Main: started consumer %d\n", consumer_id);
    int values_per_consumer = total_producer_values / num_consumers;
    int remainder = total_producer_values % num_consumers;
    int consumer_values = values_per_consumer + (consumer_id < remainder ? 1 : 0);
    printf("C%d: Started, consuming %d values\n", consumer_id, consumer_values);

    consume(&monitor, consumer_id, consumer_values);

    printf("C%d: Exiting\n", consumer_id);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    srand(time(0)); //random number generator

    if (argc != 4) {
        printf("Usage: %s <buffer_size> <num_producers> <num_consumers>\n", argv[0]);
        return 1;
    }

    int buffer_size = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    int total_producer_values = (buffer_size * 2) * num_producers;
    srand(time(NULL)); // Seed the random number generator with the current time
    int values_per_producer = total_producer_values / num_producers;

    pthread_t producers[num_producers];
    pthread_t consumers[num_consumers];

    int producer_params[num_producers][2];
    int consumer_params[num_consumers][3];

    // Initialize the monitor
    init_monitor(&monitor);

    // Create producer threads
    for (int i = 0; i < num_producers; ++i) {
        producer_params[i][0] = i;
        producer_params[i][1] = values_per_producer;
        pthread_create(&producers[i], NULL, producer, &producer_params[i]);
    }

    // Create consumer threads
    for (int i = 0; i < num_consumers; ++i) {
        consumer_params[i][0] = i;
        consumer_params[i][1] = total_producer_values;
        consumer_params[i][2] = num_consumers;
        pthread_create(&consumers[i], NULL, consumer, &consumer_params[i]);
    }

    // Join producer threads
    for (int i = 0; i < num_producers; ++i) {
        pthread_join(producers[i], NULL);
        printf("Main: producer %d joined\n", i);
    }

    // Join consumer threads
    for (int i = 0; i < num_consumers; ++i) {
        pthread_join(consumers[i], NULL);
        printf("Main: consumer %d joined\n", i);
    }

    // Destroy the monitor
    destroy_monitor(&monitor);

    printf("Main: program completed\n");

    return 0;
}

