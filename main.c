#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_LENGTH 50
#define DICTIONARY_LENGTH 466500

static char query[STRING_LENGTH], dictionary[DICTIONARY_LENGTH][STRING_LENGTH];
static int tasksCompleted = 0;

static pthread_mutex_t mutex;

struct Argument
{
    int threadID, startingIndex, lastIndex;
};

void *taskHandler(void *_argument)
{
    int i = 0;
    struct Argument *argument = (struct Argument *)_argument;

    for (i = argument->startingIndex; i <= argument->lastIndex && i <= DICTIONARY_LENGTH; i++)
    {
        if (strncmp(dictionary[i], query, strlen(query)) == 0)
        {
            printf("Thread %d: %s\n", argument->threadID, dictionary[i]);
        }
    }

    pthread_mutex_lock(&mutex);

    tasksCompleted++;

    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main()
{
    int i = 0, threads = 0, perThreadWords = 0, startingIndex = 0;

    FILE *file = NULL;
    pthread_t thread;

    struct Argument *arguments;

    printf("Enter your query: ");
    scanf("%s", query);

    printf("Enter the number of threads: ");
    scanf("%d", &threads);
    printf("\n");

    perThreadWords = (DICTIONARY_LENGTH / threads) + 1;     // calculating how many words each thread will process...
    arguments = (struct Argument *)malloc(sizeof(struct Argument) * threads);

    if ((file = fopen("dictionary.txt", "r")))      // opening dictionary file as "read-only"...
    {
        while (fscanf(file, "%s", dictionary[i]) != EOF)    // loading words from dictionary file...
        {
            i++;
        }

        fclose(file);       // releasing resource...
        pthread_mutex_init(&mutex, NULL);       // initializing mutex for critical section...

        for (i = 0; i < threads; i++)
        {
            arguments[i].threadID = i + 1;
            arguments[i].startingIndex = startingIndex;
            arguments[i].lastIndex = startingIndex + perThreadWords;

            if (pthread_create(&thread, NULL, taskHandler, &arguments[i]) != 0)
            {
                printf("error: thread creation failed...\n\n");
            }

            startingIndex += perThreadWords + 1;
        }

        while (1)       // waiting for all the threads to complete...
        {
            if (threads == tasksCompleted)
            {
                break;
            }
        }

        pthread_mutex_destroy(&mutex);       // releasing resource...
    }
    else
    {
        printf("error: unable to open the dictionary file...\n\n");
    }

    free(arguments);        // releasing resource...

    return 0;
}
