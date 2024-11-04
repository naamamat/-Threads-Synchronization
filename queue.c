#include "queue.h"
#include <stdlib.h>

Queue *CreateQueue()
{
   Queue *q = malloc(sizeof(Queue));

   if (q == NULL)
      return NULL;

   q->first = NULL;
   q->last = NULL;

   sem_init(&q->full, 0, 0);
   sem_init(&q->mutexQueue, 0, 1);

   return q;
}

void DestroyQueue(Queue *q)
{
   if (q == NULL)
      return;

   while (!IsQueueEmpty(q))
   {
      Dequeue(q);
   }

   free(q);
}

int IsQueueEmpty(Queue *q)
{
   return (q->first == NULL && q->last == NULL);
}

void Enqueue(Queue *q, void *data)
{
   sem_wait(&q->mutexQueue);
   Node *node = malloc(sizeof(Node));

   node->article = data;
   node->next = NULL;

   if (q->last == NULL)
   {
      q->first = q->last = node;
      sem_post(&q->mutexQueue);
      sem_post(&q->full);
      return;
   }

   q->last->next = node;
   q->last = node;

   sem_post(&q->mutexQueue);
   sem_post(&q->full);
}

void *Dequeue(Queue *q)
{
   sem_wait(&q->full);
   sem_wait(&q->mutexQueue);
   Node *previousHead;
   void *data;

   previousHead = q->first;

   if (previousHead == NULL)
   {
      sem_post(&q->mutexQueue);
      return NULL;
   }

   q->first = q->first->next;

   if (q->first == NULL)
      q->last = NULL;

   data = previousHead->article;
   free(previousHead);
   sem_post(&q->mutexQueue);
   return data;
}