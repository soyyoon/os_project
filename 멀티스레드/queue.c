#include <string.h>
#include "file_search.h"

void init_queue(task_queue_t* queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
}

bool enqueue(task_queue_t* queue, const char* filepath) {
    if (queue->size >= MAX_QUEUE_SIZE) return false;
    strncpy(queue->tasks[queue->rear].filepath, filepath, PATH_MAX);
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->size++;
    return true;
}

bool dequeue(task_queue_t* queue, char* out_filepath) {
    if (queue->size == 0) return false;
    strncpy(out_filepath, queue->tasks[queue->front].filepath, PATH_MAX);
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->size--;
    return true;
}