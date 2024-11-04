// Naama Matzliach 322316506
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "boundedQueue.h"
#include "queue.h"
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MaxInRow 101 // maximum of 100 chars in line in the configuration file
#define NUMTYPE 3
#define SPORT 0
#define NEWS 1
#define WHEATHER 2
#define COLOR_MAX_LETTERS 101

BoundedQueue **queueArray;
BoundedQueue *queueScreen;
Queue *queueSport;
Queue *queueNews;
Queue *queueWeather;

int currentSize;
pthread_mutex_t mutex;

typedef struct
{
    int id;
    int numLoop;
} StructForProducer;

void initializeQueueArray(int screenQueueSize)
{
    currentSize = 0;
    queueArray = (BoundedQueue **)malloc(1 * sizeof(BoundedQueue *));
    queueArray[currentSize] = NULL;

    queueSport = CreateQueue();
    queueNews = CreateQueue();
    queueWeather = CreateQueue();

    queueScreen = CreateQueueB(screenQueueSize);
}

void newToQueueArray(int maxSize)
{
    BoundedQueue *queue = CreateQueueB(maxSize);
    queueArray[currentSize] = queue;
    currentSize++;
    // add one for null
    queueArray = (BoundedQueue **)realloc(queueArray, (currentSize + 1) * sizeof(BoundedQueue *));
    queueArray[currentSize] = NULL;
}

void cleanup()
{

    for (int i = 0; i < currentSize; i++)
    {
        DestroyQueueB(queueArray[i]);
    }
    free(queueArray);

    DestroyQueue(queueSport);
    DestroyQueue(queueNews);
    DestroyQueue(queueWeather);
    DestroyQueueB(queueScreen);
}

void readConfigFile(const char *filename, int **configValues, int *numValues)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Failed to open the configuration file.\n");
        return;
    }

    char buffer[MaxInRow];
    int maxValues = 1;
    int count = 0;

    // Allocate memory for the configuration values
    *configValues = (int *)malloc(maxValues * sizeof(int));

    while (fgets(buffer, MaxInRow, file) != NULL)
    {
        // Convert the read value to an integer
        int value = atoi(buffer);
        // if (value == 0)
        // {
        //     printf("Invalid config file\n");
        //     exit(0);
        // }

        // Check if we need to resize the array
        if (count >= maxValues)
        {
            maxValues *= 2;
            *configValues = (int *)realloc(*configValues, maxValues * sizeof(int));
        }

        // Save the value in the array
        (*configValues)[count] = value;
        count++;
    }

    fclose(file);
    *numValues = count;
}

void *dispatcher()
{
    int counterDone = 0;
    int *countersDone = (int *)calloc(currentSize, sizeof(int));

    while (1)
    {
        for (int i = 0; i < currentSize; i++)
        {
            if (countersDone[i] == 1)
            {
                continue;
            }

            char *article = DequeueB(queueArray[i]);

            if (strstr(article, "SPORTS") != NULL)
            {
                Enqueue(queueSport, article);
            }
            else if (strstr(article, "NEWS") != NULL)
            {
                Enqueue(queueNews, article);
            }
            else if (strstr(article, "WEATHER") != NULL)
            {
                Enqueue(queueWeather, article);
            }
            else if (!strcmp(article, "DONE"))
            {
                countersDone[i] = 1;
                counterDone++;
            }
        }
        if (counterDone == currentSize)
        {
            break;
        }
    }

    Enqueue(queueWeather, "DONE");
    Enqueue(queueNews, "DONE");
    Enqueue(queueSport, "DONE");
    free(countersDone);
    return NULL;
}

void producer(int id, int numLoop)
{
    // set count of which type-
    int countTypeArray[NUMTYPE] = {0, 0, 0};
    char type[COLOR_MAX_LETTERS] = {0};

    int randNumber;

    // Generate a random number between 0 and 2
    srand(time(NULL));
    int num = id - 1;

    for (int i = 0; i < numLoop; i++)
    {
        randNumber = rand() % 3;
        if (randNumber == 0)
        {
            strncpy(type, "SPORTS", COLOR_MAX_LETTERS);
        }
        else if (randNumber == 1)
        {
            strncpy(type, "NEWS", COLOR_MAX_LETTERS);
        }
        else
        {
            strncpy(type, "WEATHER", COLOR_MAX_LETTERS);
        }

        // writing the stringLine:
        int lengthLine = snprintf(NULL, 0, "Producer %d %s %d", num, type, countTypeArray[randNumber]);
        char *stringLine = (char *)malloc((lengthLine + 1) * sizeof(char));
        snprintf(stringLine, lengthLine + 1, "Producer %d %s %d", num, type, countTypeArray[randNumber]++);

        // enter the stringLine:
        EnqueueB(queueArray[num], (void *)stringLine);
    }

    EnqueueB(queueArray[num], "DONE");
}

void *threadProducer(void *arg)
{
    StructForProducer *sfp = (StructForProducer *)arg;
    producer(sfp->id, sfp->numLoop);
    return NULL;
}

void *coEditor(void *queueArg)
{
    Queue *queue = (Queue *)queueArg;
    while (1)
    {
        char *article = Dequeue(queue);
        if (strcmp(article, "DONE")) // Not DONE
        {
            usleep(100000); // Editing
            EnqueueB(queueScreen, article);
        }
        else
        {
            EnqueueB(queueScreen, article);
            break;
        }
    }
    return NULL;
}

void *screenManager()
{
    int counterDone = 0;
    while (counterDone < 3)
    {
        char *article = DequeueB(queueScreen);
        if (!strcmp(article, "DONE"))
        {
            counterDone++;
        }
        else
        {
            printf("%s\n", article);
            free(article);
        }
    }
    printf("DONE\n");
    return NULL;
}

int main(int argc, char *argv[])
{
    const char *filename;
    if (argc > 1)
    {
        filename = argv[1];
    }
    else
    {
        return 0;
    }

    // read the config file:
    int *configValues;
    int numValues;
    readConfigFile(filename, &configValues, &numValues);

    initializeQueueArray(configValues[numValues - 1]);
    int numOfProducers = numValues / 4; // which producer add 4 rows to numValues.

    pthread_t *threads = malloc(numOfProducers * sizeof(pthread_t));
    StructForProducer *sfp = malloc(numOfProducers * sizeof(StructForProducer));
    for (int i = 0; i < numOfProducers; i++)
    {
        sfp[i].id = configValues[i * 4];
        sfp[i].numLoop = configValues[i * 4 + 1];
        int queueSize = configValues[i * 4 + 2];

        newToQueueArray(queueSize);
        pthread_create(&threads[i], NULL, threadProducer, &sfp[i]);
    }
    pthread_t threadForDispatcher;
    pthread_create(&threadForDispatcher, NULL, dispatcher, NULL);

    pthread_t coEditorThreads[NUMTYPE];
    for (int i = 0; i < NUMTYPE; i++)
    {
        switch (i)
        {
        case 0:
            pthread_create(&coEditorThreads[i], NULL, coEditor, queueSport);
            break;
        case 1:
            pthread_create(&coEditorThreads[i], NULL, coEditor, queueNews);
            break;
        case 2:
            pthread_create(&coEditorThreads[i], NULL, coEditor, queueWeather);
            break;
        }
    }

    pthread_t threadForScreenManager;
    pthread_create(&threadForScreenManager, NULL, screenManager, NULL);

    // Wait for all threads to finish
    for (int i = 0; i < numOfProducers; i++)
    {
        pthread_join(threads[i], NULL);
    }
    pthread_join(threadForDispatcher, NULL);

    for (int i = 0; i < NUMTYPE; i++)
    {
        pthread_join(coEditorThreads[i], NULL);
    }

    pthread_join(threadForScreenManager, NULL);

    free(threads);

    // Free configValues:
    free(configValues);

    free(sfp);

    cleanup();

    return 0;
}
