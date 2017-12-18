#ifndef __PERIODICTASK__
#define __PERIODICTASK__

/*
 *
 *
*/
// a task
typedef struct{
	int id;
	int priority;
	int cost;
	int period;
	int deadline;
	int offset;
}elem_task_T;

// task list
typedef struct periodicTask
{
	elem_task_T task;
	struct periodicTask *next;
}periodicTask_T;

// a transaction
typedef struct{
	int number;
	periodicTask_T *tasks;
}elem_trans_T;

// transaction list
typedef struct trans
{
	elem_trans_T trans;
	struct trans *next;
}transaction_T;

// a system
typedef struct
{
	int number;
	transaction_T *transactions;
}system_T;


// function concerning about task
elem_task_T create_task(int id, int priority, int cost, int period, int deadline, int offset);
void print_task(elem_task_T e);

// function concerning about transaction
elem_trans_T * create_trans();
int trans_empty(elem_trans_T trans);
int tasks_number_in_trans(elem_trans_T trans);
elem_trans_T *trans_copy(elem_trans_T trans);
void add_task_to_trans(elem_trans_T * head, elem_task_T e);
void print_transaction(elem_trans_T e);


// function concerning about system
system_T *create_system();
int system_empty(system_T s);
int trans_number_in_system(system_T s);
void add_trans_to_system(system_T *system, elem_trans_T e);
void print_system(system_T s);

// // function concerning about file
void save_to_ml(char filename[], system_T s,int U, int nbTrans, int nbTasks,int print);
void save_to_txt(char filename[],system_T s,int U, int nbTrans, int nbTasks);



#endif
