#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h> //fork
#include <sys/mman.h> //mmap
#include <sys/shm.h>
#include <sys/types.h> //pid_t,fork



#ifndef PROJ2_HEADER
#define PROJ2_HEADER

	#define ARGSMAXTIME 2000
	#define ARGSMINTIME 0

typedef struct {
		int pI; //Number of immigrants
		int iG; //Max time of generating new immigrant process
		int jG; //Max time of judge walking into the courthouse
		int iT; //Max time of getting certificate
		int jT; //Max time of getting decision
	}Arguments;
	
	Arguments arguments;
	FILE *logFile;

	//Semaphores
	sem_t *sem_immStart, //Next immigrant in line is ready to start, enter and check
		  *sem_immCheck, //All immigrants in the building have checked, judge can start confirmation
		  *sem_immGotCert, // Immigrant got the certificate and is ready to leave
		  *sem_jdgEnter, //Judge entered the building, nobody else can enter the building
		  *sem_jdgConf, //Judge ended confirmation
		  *sem_log; //Printing to the log file


	//Shared memory variables
	int *actionCounter=NULL, //A, Number of action performed
	 	*inBldNotConf=NULL, //NE,Number of immigrants that are in the building but the confirmation has not been made yet 
 		*chckdNotConf=NULL, //NC,Number of immigrants that are in the building and have checked but the confirmation has not been made yet 
		*inBld=NULL, //NB, Number of immigrants in the building
		*resolvedImmigrants=NULL; //Number of immigrants with confirmation that left the building

//________________________________FUNCTIONS___________________________________
		bool isNumber(char string[]);
		bool initArgs(int argc, char *argv[], Arguments *arguments);
		bool initLogFile();
		bool initShmSem();
		bool cleanup();
		void immigrants();
		void judge();
#endif