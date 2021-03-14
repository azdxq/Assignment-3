#include "sharedStructures.h"

int main(int argc, char *argv[])
{
        sem_t *mutex = sem_open("mutex", 0); // open semaphores 
        sem_t *full = sem_open("full", 0);
        attachMemory(); 
        srand(time(NULL)); // allow us to use rand
        int item = (rand() % 1000 );
        Output(sm->logfile, "Time is : %s - Producer created item: %d\n", createTime(), item); 
        printf("Produced item: %d\n", item);
        printf("Producer now waiting between 1 to 5 seconds inclusively\n");
        int waitTimeForProducer = rand() % 5 + 1; // randomly sleep for 1 to 5 seconds
        sleep(waitTimeForProducer);
        sm->item = item;
        sm->MonCounter++;
        sem_post(mutex); 
        sem_post(full);
        sm->ProCounter--;
return 0;
}
