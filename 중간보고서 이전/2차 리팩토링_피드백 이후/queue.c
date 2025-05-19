#include <string.h>
#include "file_search.h"

void init_queue(task_queue_t* queue) {
    queue->f = 0;
    queue->r = 0;
    queue->size = 0;
}

bool enqueue(task_queue_t* queue, const char* filepath) {
    if (queue->size >= MAX_QUEUE_SIZE) return false;
    strncpy(queue->tasks[queue->r], filepath, PATH_MAX);
    queue->r = (queue->r + 1) % MAX_QUEUE_SIZE;
    queue->size++;
    return true;
}

bool dequeue(task_queue_t* queue, char* out_filepath) {
    if (queue->size == 0) return false;
    strncpy(out_filepath, queue->tasks[queue->f], PATH_MAX);
    queue->f = (queue->f + 1) % MAX_QUEUE_SIZE;
    queue->size--;
    return true;
}
