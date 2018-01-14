#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_LENGTH 50

static char query[STRING_LENGTH], **dictionary = NULL;
static int tasksCompleted = 0, queryLength = 0, dictionaryLength = 0;

static pthread_mutex_t mutex;

struct Argument
{
    int threadID, startingIndex, lastIndex;
};

int countLines(FILE *file)
{
    char string[STRING_LENGTH];
    int lines = 0;

    while (!feof(file))
    {
        fscanf(file, "%s", string);

        lines++;
    }

    rewind(file);

    return lines;
}

void *taskHandler(void *parameter)
{
    int i = 0;
    struct Argument *argument = parameter;

    for (i = argument->startingIndex; i <= argument->lastIndex && i < dictionaryLength; i++)
    {
        if (strncmp(dictionary[i], query, queryLength) == 0)
        {
            printf("Thread %d: %s\n", argument->threadID, dictionary[i]);
        }
    }

    pthread_mutex_lock(&mutex);         // locking critical section...

    tasksCompleted++;

    pthread_mutex_unlock(&mutex);       // unlocking critical section...

    return NULL;
}

int main()
{
    int i = 0, threads = 0, perThreadWords = 0, startingIndex = 0;

    FILE *file = NULL;
    pthread_t thread;

    struct Argument *arguments;

    if ((file = fopen("dictionary.txt", "r")))      // opening dictionary file as "read-only"...
    {
        dictionaryLength = countLines(file);        // counting the number of lines...
        dictionary = malloc(sizeof(char *) * dictionaryLength);     // allocating enough memory for dictionary...

        for (i = 0; i < dictionaryLength; i++)     // allocating memory for words in dictionary...
        {
            dictionary[i] = malloc(sizeof(char) * STRING_LENGTH);
        }

        i = 0;

        while (fscanf(file, "%s", dictionary[i]) != EOF)    // loading words from dictionary file...
        {
            i++;
        }

        fclose(file);       // releasing resource...
        printf("dictionary loaded successfully...\n\nwords found: %d\n\n", dictionaryLength);
        printf("query: ");       // user input prompt...
        scanf("%s", query);       // taking user input...

        printf("number of threads: ");       // user input prompt...
        scanf("%d", &threads);       // taking user input...
        printf("\n");

        queryLength = strlen(query);        // getting query length...
        perThreadWords = (dictionaryLength / threads) + 1;     // calculating how many words each thread will process...
        arguments = malloc(sizeof(struct Argument) * threads);

        pthread_mutex_init(&mutex, NULL);       // initializing mutex for critical section...

        for (i = 0; i < threads; i++)
        {
            arguments[i].threadID = i + 1;
            arguments[i].startingIndex = startingIndex;
            arguments[i].lastIndex = startingIndex + perThreadWords;

            while (pthread_create(&thread, NULL, taskHandler, &arguments[i]) != 0)      // trying until thread created successfully...
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

        // all the cleanups are done here...
        pthread_mutex_destroy(&mutex);       // releasing resource...
        free(arguments);        // releasing resource...

        for (i = 0; i < dictionaryLength; i++)      // releasing all the words of the dictionary...
        {
            free(dictionary[i]);
        }

        free(dictionary);        // releasing resource...
    }
    else
    {
        printf("error: unable to open the dictionary file...\n\n");
    }

    return 0;
}
