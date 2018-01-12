#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define STRING_LENGTH 50
#define DICTIONARY_LENGTH 466500

static char query[STRING_LENGTH], dictionary[DICTIONARY_LENGTH][STRING_LENGTH];
static int tasksCompleted = 0, perThreadWords = 0;

static pthread_mutex_t mutex;

void *taskHandler(void *argument)
{
    int i = 0, startingIndex = (int)argument, lastIndex = startingIndex + perThreadWords;

    for (i = startingIndex; i <= lastIndex && i <= DICTIONARY_LENGTH; i++)
    {
        if (strncmp(dictionary[i], query, strlen(query)) == 0)
        {
            printf("found: %s\n", dictionary[i]);
        }
    }

    pthread_mutex_lock(&mutex);

    tasksCompleted++;

    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main()
{
    int i = 0, threads = 0, startingIndex = 0;

    FILE *file = NULL;
    pthread_t thread;

    printf("Enter your query: ");
    scanf("%s", query);

    printf("Enter the number of threads: ");
    scanf("%d", &threads);

    perThreadWords = (DICTIONARY_LENGTH / threads) + 1;     // calculating how many words each thread will process...

    if ((file = fopen("dictionary.txt", "r")))      // opening dictionary file as "read-only"...
    {
        while (fscanf(file, "%s", dictionary[i]) != EOF)    // loading words from dictionary file...
        {
            i++;
        }

        fclose(file);   // releasing resource...
        pthread_mutex_init(&mutex, NULL);

        for (i = 0; i < threads; i++)
        {
            if (pthread_create(&thread, NULL, taskHandler, (void *)startingIndex) != 0)
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
    }
    else
    {
        printf("error: unable to open the dictionary file...\n\n");
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}
