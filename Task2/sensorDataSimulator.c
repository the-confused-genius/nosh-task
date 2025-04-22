#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024
#define PRINT_SIZE 50

unsigned char buffer[MAX_BUFFER_SIZE];
int buffer_len = 0;
pthread_mutex_t buffer_mutex;

// Sensor simulator - runs every second
void* sensor_simulator(void* arg) {
    while (1) {
        sleep(1);
        int num_bytes = rand() % 6; //random number between 0 to 5
        unsigned char data[5];
        for (int i = 0; i < num_bytes; i++) {
            data[i] = rand() % 256;
        }

        pthread_mutex_lock(&buffer_mutex);
        if (buffer_len + num_bytes < MAX_BUFFER_SIZE) {
            memcpy(&buffer[buffer_len], data, num_bytes);
            buffer_len += num_bytes;
        }
        pthread_mutex_unlock(&buffer_mutex);
    }
    return NULL;
}

// Main processor - runs every 10 seconds
void* main_processor(void* arg) {
    while (1) {
        sleep(10);
        pthread_mutex_lock(&buffer_mutex);
        if (buffer_len >= PRINT_SIZE) {
            printf("Latest 50 bytes (hex): ");
            for (int i = buffer_len - PRINT_SIZE; i < buffer_len; i++) {
                printf("%02X ", buffer[i]);
            }
            printf("\n");

            // Remove the printed bytes
            buffer_len -= PRINT_SIZE;
        }
        pthread_mutex_unlock(&buffer_mutex);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t sensor_thread, processor_thread;
    pthread_mutex_init(&buffer_mutex, NULL);

    pthread_create(&sensor_thread, NULL, sensor_simulator, NULL);
    pthread_create(&processor_thread, NULL, main_processor, NULL);

    pthread_join(sensor_thread, NULL);
    pthread_join(processor_thread, NULL);

    pthread_mutex_destroy(&buffer_mutex);
    return 0;
}
