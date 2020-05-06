#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h> //fork
#include <sys/mman.h> //mmap
#include <sys/shm.h>//MAPANON
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
	/**
    *Checks if parameter of the function is integer stored in string
    *@param string Number stored in string
    *@return bool true/false if the string is number
    */
		bool isNumber(char string[]);
	/**
    *Checks if arguments of the program are valid and stores them into variables
    *@param argc Number of arguments
    *@param argv Arguments
    *@param arguments struct with variables to store arguments
    *@return bool true/false if the initialization was succesful
    */
		bool initArgs(int argc, char *argv[], Arguments *arguments);
	/**
    *Initializes a file for logs to be stored
    *@return bool true/false if the initialization was succesful
    */
		bool initLogFile();
	/**
    *Initializes shared variables and semaphores
    *@return bool true/false if the the initialization was succesful
    */
		bool initShmSem();
	/**
    *Destroys and unlinks semaphores, shared variables and log file
    *@return bool true/false error occured
    */
		bool cleanup();
	/**
    *Prints a line into log file with counters
	*@param text String to be written
	*@param idImm number of the immigrant
	*@param NE number of not checked immigrants in the building
	*@param NC number of checked immigrants in the building	
	*@param NB number of immigrants in the building
    */
		void printLogImmigrant(char *text, int idImm, int NE, int NC, int NB);
	/**
    *Prints a line into log file without counters
	*@param text String to be written
    */
		void printLogImmigrantSimple(char *text, int idImm);
	/**
    *Prints a line into log file with counters
	*@param text String to be written
    */
		void printLogJudge(char *text);
	/**
    *Prints a line into log file without counters
	*@param text String to be written
    */
		void printLogJudgeSimple(char *text);
	/**
    *Creates number of immigrants specified in arguments of the program. Uses semaphores and shared variables to control the immigrant. 
    */	
		void immigrants();
	/**
    *Creates judge. Uses semaphores and shared variables to control the judge. If there are some immigrants waiting to be confirmed, function is called recursively.
    */
		void judge();
#endif