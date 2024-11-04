#ifndef _BOUNDEDQUEUE_
#define _BOUNDEDQUEUE_
#include <semaphore.h>
typedef struct nodeB
{
  void *article;
  struct nodeB *next;
} NodeB;

typedef struct queueB
{
  NodeB *first;
  NodeB *last;
  int maxSize;
  sem_t empty;
  sem_t full;
  sem_t mutexQueue;

} BoundedQueue;

BoundedQueue *CreateQueueB(int maxSize);

void DestroyQueueB(BoundedQueue *q);

int IsQueueEmptyB(BoundedQueue *q);

void EnqueueB(BoundedQueue *q, void *d);

void *DequeueB(BoundedQueue *q);

// int QueueSizeB(BoundedQueue *q);

#endif