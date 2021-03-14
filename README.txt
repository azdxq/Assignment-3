This project is about the user inputing a number of producers and consumer so that we can solve the producer-consumer problem using monitor. The option -h displays all the options availble to the user. -o sets the name for the logfile, with a default name of logfile. -p sets the number of producers with a default of 2. -c sets the number of consumers with a default of 6. And -t sets the time until the program automatically terminates, even if it is not finished with a default time of 100 seconds. 

The formating will be monitor [-h] [-o logfile] [-p m] [-c m] [-t time]

The program will terminate if all children have finished, if the time input by -t is reached, or if the ctrl-c signal is caught. 
The program is a test using semaphores for the producer consumer problem.
To compile simply use make and then input your commands. To clean up us make clean
