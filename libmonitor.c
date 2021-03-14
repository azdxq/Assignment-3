#include "sharedStructures.h"

bool first = true;

void helpFunction()
{
    printf("Monitor app help \n\nUsage:\tmonitor [-h]\n\tmonitor [-h] [-o logfile] [-p m] [-c n] [-t time]\nOptions:\n  -o logfile Name of the file to save logs; default: logfile\n  -p m Number of producers; default: m = 2\n  -c n Number of consumers; default: n = 6\n  -t time The time in seconds after which the process will terminate, even if it has not finished. (Default: 100)\n\n");
}

void attachMemory() { // attaches the shared memory
        createMemory();
        releaseMemory();
        createMemory();
}

void removeMemory() { // removes the shared memory
        releaseMemory();
        deleteMemory();
}

void createMemory () { // creates the shared memory
        smKey = ftok("Makefile", 'p');
        if ((smID = shmget(smKey, sizeof(struct SharedMemory), IPC_CREAT | S_IRUSR | S_IWUSR)) < 0) {
                exit(EXIT_FAILURE);
        } else {
                sm = (struct SharedMemory*) shmat(smID, NULL, 0);
        }
}

void releaseMemory () { // releases the shared memory
        if (sm != NULL) {
                if (shmdt(sm)) {
                       exit(EXIT_FAILURE);
                }
        }
}

void deleteMemory() { // deletes the shared memory
        if (smID > 0) {
                if (shmctl(smID, IPC_RMID, NULL) < 0) {
                        exit(EXIT_FAILURE);
                }
        }
}

void File(char* file) { // create file
        FILE* newFile = fopen(file, "w"); 
        fclose(newFile); 
}


void produceMon(int producer) { // Monitor portion for the producer
        sem_t *mutex = sem_open("mutex", 0);
        sem_t *empty = sem_open("empty" , 0);
        sem_wait(empty);
        sem_wait(mutex);
        char id[256];
        sprintf(id, "%d", producer);
        execl("./producer", id, NULL);
}

void consumeMon(int consumer) { // monitor portion for the consumer
        sem_t *mutex = sem_open("mutex", 0);
        sem_t *full = sem_open("full" , 0);
        sem_wait(full);
        sem_wait(mutex);
        char id[256];
        sprintf(id, "%d", consumer);
        execl("./consumer", id, NULL);
}

void Output(char* file, char* i, ...) { // creates the writing and logging
        FILE* write = fopen(file, "a+"); 
        char buffer[4096];
        va_list args; 
        va_start(args, i);
        vsprintf(buffer, i, args);
        va_end(args);
        fprintf(write, buffer);
        fclose(write);
}

void signals(int sigNumber, void handler(int)) { // signal for timer and ctrl+c
	struct sigaction sa;
	if (sigemptyset(&sa.sa_mask) == -1) {
		exit(EXIT_FAILURE);
	}
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	if (sigaction(sigNumber, &sa, NULL) == -1) { 
		exit(EXIT_FAILURE);
	}
}

void signalHandler(int signal) { // handles operations of the signal
        if (sm->parentid != getpid()){
                printf("Forcing the Consumers to leave\n");
                exit(EXIT_FAILURE);
        }

        if (sm->parentid == getpid() && (signal == SIGALRM || signal == SIGUSR1)) { 
                printf("Timer has reached 0, remaining consumers are now exiting\n");
       }
        Output(sm->logfile, "Time is : %s - Ending process because of the signal\n", createTime()); 
        killpg(sm->pgid, signal == SIGALRM ? SIGUSR1 : SIGINT);
        while (wait(NULL) > 0); 
        Output(sm->logfile, "Time is : %s - Shared Memory is Deallocated\n", createTime());
        removeMemory(); 
        printf("Monitor is now leaving\n");
        sem_unlink("mutex");
        sem_unlink("empty");
        sem_unlink("full");
        exit(EXIT_SUCCESS);

}

void createProducer(int producer, int i) { // create the producer
        pid_t pid = fork();
        if (pid == 0 && i == 0 && first == true) { 
                first = false;
                sm->pgid = getpid();
        }
        setpgid(pid, sm->pgid);
        if (pid == 0) { 
                signals(SIGUSR1, signalHandler);
                produceMon(producer);
                exit(EXIT_SUCCESS);
        }
}

void createConsumer(int consumer, int i) { // create the consumer
        pid_t pid = fork();
        while (pid == 0 && !first) {
                sleep(0);
        }
        setpgid(pid, sm->pgid);
        if (pid == 0) { 
                signals(SIGUSR1, signalHandler);
                consumeMon(consumer);
                exit(EXIT_SUCCESS);
        }
}


char* createTime() { // creates the time used for logfile
        char* createdTime = malloc(FORMATTED_TIME_SIZE * sizeof(char));
        time_t now = time(NULL);
        strftime(createdTime, FORMATTED_TIME_SIZE, FORMATTED_TIME_FORMAT, localtime(&now));
        return createdTime;
}

void Timer(const int t) {
        signals(SIGALRM, signalHandler);
        signals(SIGUSR1, signalHandler);

        struct itimerval timer;
        timer.it_value.tv_sec = t;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;

        if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
                perror("failed to set timer");
                exit(EXIT_FAILURE);
        }
}

