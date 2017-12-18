#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "PeriodicTask.h"

// task id starting from 1
int NUMBER = 1;

/*
 * function create task
 * 
 */

elem_task_T create_task(int id, int priority, int cost, int period, int deadline, int offset){
	elem_task_T t2;
	t2.id = id;
	t2.priority = priority;
	t2.cost = cost;
	t2.period = period;
	t2.deadline = deadline;
	t2.offset = offset;
	return t2;
}

void print_task(elem_task_T e){
	printf("%d\t%d\t\t%d\t%d\t%d\t\t%d\n",e.id,e.priority, e.cost,e.period,e.deadline, e.offset);
}




int trans_empty(elem_trans_T trans){
	return (!trans.number);
}

int tasks_number_in_trans(elem_trans_T trans){
	return trans.number;
}

elem_trans_T * create_trans(){
	elem_trans_T *trans;
	trans = malloc(sizeof(elem_trans_T));
	trans->number=0;
	trans->tasks=NULL;
	trans;
}

elem_trans_T *trans_copy(elem_trans_T trans){
	elem_trans_T *trans_copy;
	trans_copy = create_trans();
	if(trans_empty(trans)) return trans_copy;
	else{
		trans_copy->number=trans.number;
		trans_copy->tasks = malloc(sizeof(periodicTask_T));
		periodicTask_T * current = trans_copy->tasks;
		current->task = trans.tasks->task;
		periodicTask_T * pre = trans.tasks;
		while (pre->next!=NULL){
			current->next = malloc(sizeof(periodicTask_T));
			current->next->task = pre->next->task;
			pre=pre->next;
			current = current->next;
		}
	}
	return trans_copy;	
	
}

void add_task_to_trans(elem_trans_T * trans, elem_task_T t) {
	if(trans_empty(*trans)){
		trans->tasks = malloc(sizeof(periodicTask_T));
		trans->tasks->task = t;
		trans->tasks->next = NULL;
		trans->number++;
	}else{
		periodicTask_T * current = trans->tasks;		
		while (current->next != NULL) {
		current = current->next;
		}
		current->next = malloc(sizeof(periodicTask_T));
		current->next->task = t;
		current->next->next = NULL;
		trans->number++;
	}
	
	NUMBER++;
}

void print_transaction(elem_trans_T e){
	if(!trans_empty(e)){
		periodicTask_T *current = e.tasks;
		printf("id\tpriority\tcost\tperiod\tdeadline\toffset\n");
		while (current != NULL) {
			print_task(current->task);
			current = current->next;
	    	}
	}
}



system_T *create_system(){
	system_T *system;
	system = malloc(sizeof(system_T));
	system->number=0;
	system->transactions=NULL;
	return system;
}

int system_empty(system_T s){
	return (!s.number);
}

int trans_number_in_system(system_T s){
	return s.number;
}

void add_trans_to_system(system_T *system, elem_trans_T trans){
	if(system_empty(*system)){
		system->transactions = malloc(sizeof(transaction_T));
		system->transactions->trans = trans;
		system->transactions->next = NULL;
		system->number++;
	}else{
		transaction_T * current = system->transactions;		
		while (current->next != NULL) {
		current = current->next;
		}
		current->next = malloc(sizeof(transaction_T));
		current->next->trans = trans;
		current->next->next = NULL;
		system->number++;
	}
		
}


void print_system(system_T s){
	if(!system_empty(s)){
		transaction_T *current = s.transactions;
		int i=1;
		while (current != NULL) {
			printf("\n*************************** transaction %d ***************************\n", i);
			print_transaction(current->trans);
			current = current->next;
			i++;
	    	}
	}
}








void save_to_ml(char filename[], system_T s,int U, int nbTrans, int nbTasks,int print){
	char name[50];
	strcpy(name,filename);
	strcat(name,".ml");
	FILE *file = fopen(name, "w");
	if (file==NULL) {
	printf("Failed to open %s\n", name);
	}
	// load library
	fprintf(file, "open Analyzer_offset;;\n\n");
	// variables
	fprintf(file, "let u = %d;;\n\n", U);
	fprintf(file, "let nbTrans = %d;;\n\n", nbTrans);
	fprintf(file, "let nbTasks = %d;;\n\n",nbTasks);
	int i=0,trans_nbchar=10*nbTasks, system_nbchar=10*nbTrans;
	char trans_c[trans_nbchar], task_id[10], system_c[system_nbchar],trans_id[10];
	
	transaction_T * current_trans = s.transactions;
	while(current_trans!=NULL){
		
		strcpy(trans_c,"[");
		periodicTask_T * current_task = current_trans->trans.tasks;
		int j = 0;
		while (current_task != NULL) {
		fprintf(file, "let task%d ={task_id=%d; task_priority= %d;task_cost= %d;task_period= %d;task_deadline= %d;task_offset= %d};;\n",
		current_task->task.id,current_task->task.id,current_task->task.priority,current_task->task.cost,
		current_task->task.period,current_task->task.deadline,current_task->task.offset);

		if(j==0)sprintf(task_id,"task%d",current_task->task.id);
		else sprintf(task_id,";task%d",current_task->task.id);
		strcat(trans_c,task_id);
		current_task = current_task->next;
		j++;
		
		}
		// build system contenu
		if(i==0) {
		strcpy(system_c,"[");
		sprintf(trans_id,"trans%d",i);
		}
		else sprintf(trans_id,";trans%d",i);
		strcat(system_c, trans_id);
		fprintf(file,"let trans%d =%s];;\n\n", i, trans_c);
		current_trans = current_trans->next;
		i++;
		
	}
	fprintf(file,"let system =%s];;\n\n", system_c);
	
	fprintf(file,"let infos = analyzer_certified_offset_all_infos system;;\n\n");

	//print variable
	fprintf(file,"print_int(u);;\n\n");
	fprintf(file,"print_string \"\\t\";;\n\n");

	fprintf(file,"print_int(nbTrans);;\n\n");
	fprintf(file,"print_string \"\\t\";;\n\n");

	fprintf(file,"print_int(nbTasks);;\n\n");
	fprintf(file,"print_string \"\\t\";;\n\n");

	// function mesure time
	fprintf(file,"let time1 analyzer_certified_offset x = let t = Sys.time() in let fx = analyzer_certified_offset x in Printf.printf \"%%f\\t\" (Sys.time() -. t); fx;;\n\n");
	fprintf(file,"let n1= time1 analyzer_certified_offset system;;\n\n");
	fprintf(file,"print_string (string_of_bool n1);;\n");
	fprintf(file,"print_string \"\\t\";;\n\n");

	fprintf(file,"let time2 certifier_offset x= let t = Sys.time() in let fx = certifier_offset x in Printf.printf \"%%f\\t\" (Sys.time() -. t); fx;;\n\n");
	
	// function analyzer infos
	fprintf(file,"let n2= time2 certifier_offset infos;;\n\n");
	fprintf(file,"print_string (string_of_bool n2);;\n");
	fprintf(file,"print_string \"\\t\\n\";;\n\n");

	// Flag
	fprintf(file,"let () = let oc = open_out \"Flag\" in Printf.fprintf oc \"%%s\" (string_of_bool n2); close_out oc;;\n\n");

	// function print result
	//fprintf(file,"print_string \"\\nschedulability: \";;\n\n");
	//fprintf(file,"print_string (string_of_bool n1);;\n");
	//fprintf(file,"print_string \"\\n\";;\n\n");
	
	//fprintf(file,"print_string \"certifier: \";;\n\n");
	//fprintf(file,"print_string (string_of_bool n2);;\n");
	//fprintf(file,"print_string \"\\n\";;\n\n");
	

	fclose(file);
	if (print==0)
	printf("save tasks to the file %s\n",name);

}


void save_to_txt(char filename[],system_T s,int U, int nbTrans, int nbTasks){
	char name[50];
	strcpy(name,filename);
	strcat(name,".txt");

	FILE *file = fopen(name, "w");
	if (file==NULL) {
	printf("Failed to open %s\n", name);
	}
	fprintf(file, "%d %d %d\n", U, nbTrans,nbTasks);

	transaction_T * current_trans = s.transactions;
	while(current_trans!=NULL){
		
		periodicTask_T * current_task = current_trans->trans.tasks;

		while (current_task != NULL) {
		fprintf(file, "%d %d %d %d %d %d\n",
		current_task->task.id,current_task->task.priority,current_task->task.cost,current_task->task.period,
		current_task->task.deadline,current_task->task.offset);
		current_task = current_task->next;
		}
		current_trans = current_trans->next;
	}
	fclose(file);
	printf("save tasks to the file %s\n",name);

}










