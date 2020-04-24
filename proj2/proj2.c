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

	if (sem_init(sem_immStart, 1, 0) == -1) return false;
	if (sem_init(sem_immCheck, 1, 0) == -1) return false;
	if (sem_init(sem_immGotCert, 1, 0) == -1) return false;
	if (sem_init(sem_jdgEnter, 1, 0) == -1) return false;
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
	fprintf(logFile,"%d:JUDGE:%s\n",(*actionCounter)++,text);
	sem_post(sem_log);
	
}

void immigrants(){

}
void judge(){
	if (arguments.jG != 0){
		int delay= rand() % arguments.jG;
		usleep(delay * 1000);
		//sem_wait(sem_jdgEnter);
		printLogJudge("wants to enter");
		printLogJudge("waits for imm");
		printLogJudge("starts confirmation");
		printLogJudge("ends confirmation");
		printLogJudge("leaves");
	} 
	
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
			//immigrant Generator
			//immigrants();
		}
		else if(secondFork>0){
			//judge Generator
			judge();
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
