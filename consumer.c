#include "sharedStructures.h"

int main(int argc, char *argv[])
{
        sem_t *mutex = sem_open("mutex", 0); 
        sem_t *empty = sem_open("empty", 0);
        attachMemory();
        
	printf("Consumer now sleeping between 1 and 10 seconds inclusively.\n\n");
        srand(time(NULL)); // allow us to use rand
	int waitTimeForConsumer = rand() % 10 + 1; // sleep for 1 to 10 seconds
        sleep(waitTimeForConsumer);
	printf("Consumer has slept for %d seconds\n\n", waitTimeForConsumer);
	
	int conItem = sm->item; // give conItem value of shared memory item
        Output(sm->logfile, "Time is : %s - Consumer removed item: %d\n", createTime(), conItem);
        printf("Consumed item: %d\n\n", sm->item);
        
	sm->item = 0; // removes the item
        sm->MonCounter--;
        sem_post(mutex); 
        sem_post(empty);
        
 return 0;
}

