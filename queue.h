#ifndef _QUEUE_
#define _QUEUE_

#include <semaphore.h>

typedef struct node
{
  void *article;
  struct node *next;
} Node;

typedef struct queue
{
  Node *first;
  Node *last;
  sem_t full;
  sem_t mutexQueue;
} Queue;

Queue *CreateQueue();

void DestroyQueue(Queue *q);

int IsQueueEmpty(Queue *q);

void Enqueue(Queue *q, void *d);

void *Dequeue(Queue *q);

#endif