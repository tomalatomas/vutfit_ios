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
	if ((sem_immStart=mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) return false;
	if ((sem_immCheck =mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)return false;
	if ((sem_immGotCert =mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) return false;
	if ((sem_jdgEnter =mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) return false;
	if ((sem_jdgConf =mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) return false;
	if ((sem_log =mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED) return false;

	if (sem_init(sem_immStart, 1, 1) == -1) return false;
	if (sem_init(sem_immCheck, 1, 1) == -1) return false;
	if (sem_init(sem_immGotCert, 1, 0) == -1) return false;
	if (sem_init(sem_jdgEnter, 1, 1) == -1) return false;
	if (sem_init(sem_jdgConf, 1, 0) == -1) return false;
	if (sem_init(sem_log, 1, 1) == -1) return false;

	return true;
}

bool cleanup(){
	//Semaphores
	if(sem_destroy(sem_immStart) == -1) return false;
	if(sem_destroy(sem_immCheck) == -1) return false;
	if(sem_destroy(sem_immGotCert) == -1) return false;
	if(sem_destroy(sem_jdgEnter) == -1) return false;
	if(sem_destroy(sem_jdgConf) == -1) return false;
	if(sem_destroy(sem_log) == -1) return false;
	munmap(sem_immStart,sizeof(sem_t));	
	munmap(sem_immCheck,sizeof(sem_t));
	munmap(sem_immGotCert,sizeof(sem_t));
	munmap(sem_jdgEnter,sizeof(sem_t));
	munmap(sem_jdgConf,sizeof(sem_t));
	munmap(sem_log,sizeof(sem_t));


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

	sem_wait(sem_log);
		fprintf(logFile,"%d\t: JUDGE \t: %-17s \t: %d\t: %d\t: %d\n",(*actionCounter)++,text,*inBldNotConf,*chckdNotConf,*inBld);
	sem_post(sem_log);
	
}

void printLogJudgeSimple(char *text){

	sem_wait(sem_log);
		fprintf(logFile,"%d\t: JUDGE \t: %-17s \n",(*actionCounter)++,text);
	sem_post(sem_log);
	
}

void printLogImmigrant(char *text, int idImm, int NE, int NC, int NB){
	sem_wait(sem_log);
		fprintf(logFile,"%d\t: IMM %d \t: %-17s \t: %d\t: %d\t: %d\n",(*actionCounter)++,idImm,text, NE,NC,NB);
	sem_post(sem_log);
	
}

void printLogImmigrantSimple(char *text, int idImm){
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
	srand(time(NULL)*getpid());
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
				//printDebugSemValue("Immigrant:ImmStart There is someone to be confirmed",sem_immStart);
				sem_post(sem_immStart);
			sem_wait(sem_immCheck);
				printLogImmigrant("checks",i,*inBldNotConf,++(*chckdNotConf),*inBld);
			//printf("%d",i);
			sem_post(sem_immCheck);
			//printDebugSemValue("Immigrant:waiting for confirmation",sem_jdgConf);
			sem_wait(sem_jdgConf);//Continues if judge confirmed the certificate
			sem_wait(sem_immStart);
				//printDebugSemValue("Immigrant:ImmStart The someone left",sem_immStart);
				//printf("%d",i);
				//printDebugSemValue("Immigrant:jdgConf continues",sem_jdgConf);
				printLogImmigrant("wants certificate",i,*inBldNotConf,*chckdNotConf,*inBld);
				waitFor(arguments.iT);
				//(*resolvedImmigrants)++;
				printLogImmigrant("got certificate",i,*inBldNotConf,*chckdNotConf,*inBld);
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
	srand(time(NULL));
	waitFor(arguments.jG);
	    sem_wait(sem_jdgEnter);
			printLogJudgeSimple("wants to enter");
			printLogJudge("enters");
			if(*chckdNotConf!=*inBld){
				printLogJudge("waits for imm"); //Prints if some immigrant havent checked yet
			}
		sem_wait(sem_immCheck);  //Judge waits till all immigrants have checked
		sem_post(sem_immCheck);
			printLogJudge("starts confirmation");
			for(int i=0;i<*inBldNotConf;i++){
				sem_post(sem_jdgConf);
				//printDebugSemValue("Judge:made one confirmation, jdgConf:",sem_jdgConf);
 
			}

			//Judge confirmed the certificate
			//printDebugSemValue("Judge:jdgConf conf",sem_jdgConf);
			waitFor(arguments.jT);
			printLogJudge("ends confirmation");
			*inBldNotConf=0;
			*resolvedImmigrants+=*chckdNotConf;
			*chckdNotConf=0;
				waitFor(arguments.jT);
			printLogJudge("leaves");
		sem_post(sem_jdgEnter);
		if(arguments.pI==*resolvedImmigrants){
				//printf("%d = %d\n",*resolvedImmigrants,arguments.pI);
				printLogJudgeSimple("finishes");
		    }
		else
			{
				//printf("%d != %d\n",*resolvedImmigrants,arguments.pI);
				//printDebugSemValue("Judge: New judge:",sem_immStart);
				sem_wait(sem_immStart);
				sem_post(sem_immStart);
				judge();
			}
		exit(0);
}

//_________________________________________________________________________________
int main(int argc, char *argv[]){
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
	return 0;
}