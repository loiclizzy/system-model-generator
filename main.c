#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include "PeriodicTask.h"
#include "generer_tasks.h"



int main(int argc, char*argv[]){
	if(argc<4){
	if(strcmp(argv[1], "--help"))
	printf("need more input parameters!\n");
	printf("./generator [Utilization] [Nb transaction] [Nb tasks] [Distribution] [Period method] [*Print tasks]\n");
	printf("[Utilization]: int. (1-100) Utilization of system\n");
	printf("[Nb transaction]: int. Number of transaction in system\n");
	printf("[Nb tasks]: int. Number of tasks in each transaction\n");
	printf("[Distribution*]: int. [0: randomly | 1: equally]\n");
	printf("[Period method*]: int. [0: random from (1000, 10000) | 1: harmonics]\n");
	printf("[*Print tasks]: int. 0 print; else no print!\n");
	return -1;
	}

	// set a seed for random
	srand(time(NULL));

	// intialization of variables
	int U=atoi(argv[1]), nbTrans=atoi(argv[2]), nbTasks=atoi(argv[3]), print =0;
	int gType = 0, pT = 0;
	if(argc>=5) gType=atoi(argv[4]);
	if(argc>=6) pT=atoi(argv[5]);
	if(argc>=7) print=atoi(argv[6]);

	// create a system
	system_T *system;
	system = create_system();

	// generate periodic tasks
	generer_periodicTasks(system, U, nbTrans, nbTasks, gType,pT,print);
	
	// print the generation results in the terminal
	if(print==0){
	print_system(*system);
	// save to a file .txt
	char name[40];
	strcat(name, "_");
	strcat(name, argv[1]);
	strcat(name, "_");
	strcat(name, argv[2]);
	strcat(name, "_");
	strcat(name, argv[3]);
	save_to_txt(name,*system,U,nbTrans,nbTasks);
	}

	// save to a file .ml which is ready to compile
	
	save_to_ml("Generator",*system,U,nbTrans,nbTasks,print);

		
	


	return 0;

}
