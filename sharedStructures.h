#include <ctype.h> //headers
#include <getopt.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>

#define FORMATTED_TIME_SIZE 50 
#define FORMATTED_TIME_FORMAT "%H:%M:%S"

struct SharedMemory {
        size_t Pro;
        size_t Con;
        size_t total; 
        char logfile[20];
        int ProCounter;
        int ConCounter;
        int MonCounter;
        int item;
        pid_t parentid;
        pid_t pgid; 
};


char* logfilename; 

// shared memory 

int smKey; 
int smID;
struct SharedMemory* sm;
sem_t mutex, empty, full;

// functions used in libmonitor.c

void createMemory();
void attachMemory();
void releaseMemory();
void deleteMemory();
void removeMemory();
void File(char*); 
void Output(char*, char*, ...);
void sigact(int, void(int));
void signalHandler(int);
char* createTime();
void produceMon(int);
void consumeMon(int);
void createProducer(int, int);
void createConsumer(int, int);

