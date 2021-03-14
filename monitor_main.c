// Andrew Ziggas
// 3/14/2021
// Take command arguments for time, producers, and consumers to solve the consumer-producer problem

#include "sharedStructures.h"

void Timer(int);

int main(int argc, char* argv[])
{
    int opt;
    char logfile[] = "logfile"; // default name for logfile
    int countOfProducers = 2; // default values
    int countOfConsumers = 6; 
    int timeOfProcess = 100; 
      
    while ((opt = getopt(argc, argv, "o:p:c:t:h")) != -1) { // options for command line arguments
        switch (opt) {
            case 'h':
                helpFunction();
                return EXIT_SUCCESS;
            case 'o':
               strcpy(logfile,optarg);
		break;
            case 'p':
                countOfProducers = atoi(optarg);
                break;
            case 'c':
                countOfConsumers = atoi(optarg);
                break;
            case 't':
                timeOfProcess = atoi(optarg);
                break;
           default:   
                perror ("Something is wrong with argument, here is a list of options available to you: \n");
                helpFunction();
                return EXIT_FAILURE;
        }
   }

	signals(SIGINT, signalHandler); // create signal handler
	attachMemory(); // attaching the shared memory
	Output(logfile, "Time is : %s - Shared Memory Allocated\n", createTime());
	strcpy(sm->logfile, logfile); // copying filename to memory
	Timer(timeOfProcess); 
        Output(logfile, "Time is : %s - Alarm will end in %d seconds \n", createTime(),timeOfProcess);
        sem_t *mutex = sem_open("mutex", O_CREAT, 0600, 1);
        sem_t *empty = sem_open("empty", O_CREAT, 0600, 1);
        sem_t *full = sem_open("full", O_CREAT, 0600, 0);
	
        sm->ProCounter = 0;
        sm->ConCounter = 0;
        sm->MonCounter = 0;

        int i = 0; // check to see if we run out of consumers, if we do end program
        int j = 0;
        if (countOfProducers >= countOfConsumers) { // make sure consumers is > producers
                countOfConsumers = countOfProducers + 1;
        }

        int max = countOfConsumers + countOfProducers;
        if (max > 20) { // if user adds more than 20 just switch to default values
                countOfConsumers = 2;
                countOfProducers = 6;
                max = countOfConsumers + countOfProducers;
        }

        sm->Pro = countOfProducers;
        sm->Con = countOfConsumers;
        sm->total = max;
        sm->parentid = getpid();
 
        while (true) { 
                if (sm->ProCounter < countOfProducers) { 
                        createProducer(sm->ProCounter++, i);
                        i++;
                }
                if (sm->ConCounter < countOfConsumers) {
                        createConsumer(sm->ConCounter++, j);
                        j++;
                }
               if (!(sm->ConCounter < countOfConsumers) && !(sm->ProCounter < countOfProducers)) {
                        wait(NULL);
			if (i == j) {
				break;
			}
                }
        }

        while(wait(NULL) > 0); // wait for all to end

        sem_close(mutex);
        sem_close(empty);
        sem_close(full);
	printf("All Consumers have consumed\n");
	removeMemory(); 
        Output(logfile, "Time is : %s - Shared Memory Deallocated\n", createTime());
        return EXIT_SUCCESS;
}

