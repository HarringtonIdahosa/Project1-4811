// prod_cons_MT.h

#ifndef PROD_CONS_MT_H
#define PROD_CONS_MT_H

#define BUFFER_SIZE 3

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;   // Index to write to the buffer
    int out;  // Index to read from the buffer
} Monitor;

void init_monitor(Monitor* monitor);
void produce(Monitor* monitor, int producer_id, int num_values);
void consume(Monitor* monitor, int consumer_id, int num_values);
void destroy_monitor(Monitor* monitor);

#endif  // PROD_CONS_MT_H
