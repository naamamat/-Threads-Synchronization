#include "boundedQueue.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

BoundedQueue *CreateQueueB(int maxSize)
{
   BoundedQueue *q = malloc(sizeof(BoundedQueue));

   if (q == NULL)
      return NULL;

   q->first = NULL;
   q->last = NULL;
   q->maxSize = maxSize;
   sem_init(&q->full, 0, 0);
   sem_init(&q->empty, 0, maxSize);
   sem_init(&q->mutexQueue, 0, 1);
   return q;
}

void DestroyQueueB(BoundedQueue *q)
{
   if (q == NULL)
      return;

   while (!IsQueueEmptyB(q))
   {
      DequeueB(q);
   }

   free(q);
}

int IsQueueEmptyB(BoundedQueue *q)
{
   return (q->first == NULL && q->last == NULL);
}

void EnqueueB(BoundedQueue *q, void *data)
{
   sem_wait(&q->empty);
   sem_wait(&q->mutexQueue);
   NodeB *nodeB = malloc(sizeof(NodeB));

   nodeB->article = data;
   nodeB->next = NULL;


   if (q->last == NULL)
   {
      q->first = q->last = nodeB;
      sem_post(&q->mutexQueue);
      sem_post(&q->full);

      return;
   }

   q->last->next = nodeB;
   q->last = nodeB;
   sem_post(&q->mutexQueue);
   sem_post(&q->full);
}

void *DequeueB(BoundedQueue *q)
{
   sem_wait(&q->full);
   sem_wait(&q->mutexQueue);
   NodeB *previousHead;
   void *data;

   previousHead = q->first;

   if (previousHead == NULL)
   {
      sem_post(&q->mutexQueue);
      sem_post(&q->empty);
      return NULL;
   }

   q->first = q->first->next;

   if (q->first == NULL)
      q->last = NULL;

   data = previousHead->article;
   free(previousHead);
   sem_post(&q->mutexQueue);
   sem_post(&q->empty);
   return data;
}


