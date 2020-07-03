#include "proj2.h"

bool isNumber(char string[]) 
{ 
	int length = strlen(string);
	for (int i = 0; i < length; i++) {
		if (string[i]< '0' || string[i] > '9'){ 
			return false; 
		}
	}
	return true; 
} 

bool initArgs(int argc, char *argv[], Arguments *arguments){
	if(argc!=6){
		fprintf(stderr,"Invalid arguments!\n");
		return false; 	
	}
	for (int i = 1; i < argc; i++) {
		if (!isNumber(argv[i])){ 
			fprintf(stderr,"Invalid arguments!\n");
			return false; 	
		}
		if(atoi(argv[1])<1){
			fprintf(stderr,"Invalid arguments!\n");
			return false; 	
		}
		if(i>1){
			if(atoi(argv[i])<ARGSMINTIME || atoi(argv[i])>ARGSMAXTIME ){
				fprintf(stderr,"Invalid arguments!\n");
				return false; 	
			}
		}
	}	
	arguments->pI=atoi(argv[1]);
	arguments->iG=atoi(argv[2]);
	arguments->jG=atoi(argv[3]);
	arguments->iT=atoi(argv[4]);
	arguments->jT=atoi(argv[5]);
	return true; 	
}

bool initLogFile(){
	if ((logFile = fopen("proj2.out", "w")) == NULL) {
		fprintf(stderr, "Failed to open or create log file");
		return false;
	}
	setbuf(logFile, NULL);
	return true;
}
bool initShmSem(){

	//Shared Mem
	if ((actionCounter= (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) return false;
	if ((inBldNotConf= (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) return false;
	if ((chckdNotConf= (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) return false;
	if ((inBld= (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) return false;
	if ((resolvedImmigrants= (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) return false;
	(*actionCounter)=1;
	(*inBldNotConf)=0;
	(*chckdNotConf)=0;
	(*inBld)=0;
	(*resolvedImmigrants)=0;
	//Semaphores
	sem_immStart=sem_open("/xtomal02_sem_immStart",O_CREAT | O_EXCL, 0644, 1);
	sem_immCheck=sem_open("/xtomal02_sem_immCheck",O_CREAT | O_EXCL, 0644, 1);
	sem_jdgEnter=sem_open("/xtomal02_sem_jdgEnter",O_CREAT | O_EXCL, 0644, 1);
	sem_jdgLeft=sem_open("/xtomal02_sem_jdgLeft",O_CREAT | O_EXCL, 0644, 1);
	sem_jdgConf=sem_open("/xtomal02_sem_jdgConf",O_CREAT | O_EXCL, 0644, 0);
	sem_log=sem_open("/xtomal02_sem_log",O_CREAT | O_EXCL, 0644, 1);

	if (sem_immStart == SEM_FAILED) return false;
	if (sem_immCheck == SEM_FAILED) return false;
	if (sem_jdgEnter== SEM_FAILED) return false;
	if (sem_jdgLeft== SEM_FAILED) return false;
	if (sem_jdgConf== SEM_FAILED) return false;
	if (sem_log== SEM_FAILED) return false;

	return true;
}

bool cleanup(){
	//Semaphores
	sem_close(sem_immStart);
	sem_close(sem_immCheck);
	sem_close(sem_jdgEnter);
	sem_close(sem_jdgLeft);
	sem_close(sem_jdgConf);
	sem_close(sem_log);
	sem_unlink("/xtomal02_sem_immStart");
	sem_unlink("/xtomal02_sem_immCheck");
	sem_unlink("/xtomal02_sem_jdgEnter");
	sem_unlink("/xtomal02_sem_jdgLeft");
	sem_unlink("/xtomal02_sem_jdgConf");
	sem_unlink("/xtomal02_sem_log");
	//Shared memory
	munmap(actionCounter,sizeof(int));
	munmap(inBldNotConf,sizeof(int));
	munmap(chckdNotConf,sizeof(int));
	munmap(inBld,sizeof(int));
	munmap(resolvedImmigrants,sizeof(int));
	//Log file
	fclose(logFile);
	return true;
}

void printLogJudge(char *text){
	//Check issues in this repository!!!
	sem_wait(sem_log);
	fprintf(logFile,"%d\t: JUDGE \t: %-17s \t: %d\t: %d\t: %d\n",(*actionCounter)++,text,*inBldNotConf,*chckdNotConf,*inBld);
	sem_post(sem_log);
}

void printLogJudgeSimple(char *text){
	//Check issues in this repository!!!
	sem_wait(sem_log);
	fprintf(logFile,"%d\t: JUDGE \t: %-17s \n",(*actionCounter)++,text);
	sem_post(sem_log);
}

void printLogImmigrant(char *text, int idImm, int NE, int NC, int NB){
	//Check issues in this repository!!!
	sem_wait(sem_log);
	fprintf(logFile,"%d\t: IMM %d \t: %-17s \t: %d\t: %d\t: %d\n",(*actionCounter)++,idImm,text, NE,NC,NB);
	sem_post(sem_log);
}

void printLogImmigrantSimple(char *text, int idImm){
	//Check issues in this repository!!!
	sem_wait(sem_log);
	fprintf(logFile,"%d\t: IMM %d \t: %-17s \n",(*actionCounter)++,idImm,text);
	sem_post(sem_log);
}

void printDebugSemValue(char *text, sem_t *semaphore){
	int semValue;
	sem_getvalue(semaphore,&semValue);
	printf("%s:%d\n",text,semValue);
}

void waitFor(int sharedVariable){
	if(sharedVariable!=0){
		int delay= random() % sharedVariable;
		usleep(delay * 1000);
	}
}

void immigrants(){
	pid_t immigrantPid;
	int i;
	for(i=1; i<=arguments.pI; i++){
		waitFor(arguments.iG);
		immigrantPid= fork();
		if(immigrantPid==0){ //New Immigrant
			printLogImmigrantSimple("starts",i);
			sem_wait(sem_jdgEnter); //Entes building if there is no judge in building
			sem_post(sem_jdgEnter); //Preserve the value of the semaphore
			printLogImmigrant("enters",i,++(*inBldNotConf),*chckdNotConf,++(*inBld));
			sem_post(sem_immStart);
			sem_wait(sem_immCheck);
			printLogImmigrant("checks",i,*inBldNotConf,++(*chckdNotConf),*inBld);
			sem_post(sem_immCheck);
			sem_wait(sem_jdgConf);//Continues if judge confirmed the certificate
			sem_wait(sem_immStart);
			printLogImmigrant("wants certificate",i,*inBldNotConf,*chckdNotConf,*inBld);
			waitFor(arguments.iT);
			printLogImmigrant("got certificate",i,*inBldNotConf,*chckdNotConf,*inBld);
				sem_wait(sem_jdgLeft); //Entes building if there is no judge in building
				sem_post(sem_jdgLeft);
				printLogImmigrant("leaves",i,*inBldNotConf,*chckdNotConf,--(*inBld));
				exit(0);
			}
			else if (immigrantPid<0){
				fprintf(stderr, "Immigrant fork failed!\n");
				cleanup();
				exit(1);
			}
		}
	}

	void judge(){
		waitFor(arguments.jG);
		sem_wait(sem_jdgEnter);
		sem_wait(sem_jdgLeft);
		printLogJudgeSimple("wants to enter");
		printLogJudge("enters");
		if(*chckdNotConf!=*inBldNotConf){
				printLogJudge("waits for imm"); //Prints if some immigrant havent checked yet
			}
		sem_wait(sem_immCheck);  //Judge waits till all immigrants have checked
		sem_post(sem_immCheck);
		printLogJudge("starts confirmation");
		int confirmed=*inBldNotConf;
			//Judge confirmed the certificate
		waitFor(arguments.jT);
		*resolvedImmigrants+=*chckdNotConf;
		*chckdNotConf=0;
		*inBldNotConf=0;
		printLogJudge("ends confirmation");
		for(int i=0;i<confirmed;i++){
			sem_post(sem_jdgConf);
		}
		waitFor(arguments.jT);
		printLogJudge("leaves");
		sem_post(sem_jdgLeft);	
		sem_post(sem_jdgEnter);
		if(arguments.pI==*resolvedImmigrants){
			printLogJudgeSimple("finishes");
		}
		else
		{
			sem_wait(sem_immStart);
			sem_post(sem_immStart);
			judge();
		}
		exit(0);
	}

//_________________________________________________________________________________
	int main(int argc, char *argv[]){
		srand(time(NULL));
		if(!initArgs(argc,argv,&arguments)||!initLogFile()||!initShmSem()){
			fprintf(stderr, "Program initialized unsuccessfuly\n");
			return 1;
		} 
		pid_t firstFork = fork();
		pid_t secondFork=0;
		if (firstFork == 0) {
			secondFork=fork();
			if (secondFork == 0) {
			//judge Generator
				judge();
			}
			else if(secondFork>0){
			//immigrant Generator
				immigrants();
			}
			else{
				fprintf(stderr, "Fork failed!");
				return 1;	
			}
		}	
		else if(firstFork<0){
			fprintf(stderr, "Fork failed!");
			return 1;
		}
	else{ //MAIN PROCCESS
		cleanup();
	}
	//cleanup();
	return 0;
}
