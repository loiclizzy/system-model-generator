#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "PeriodicTask.h"

extern int NUMBER;

int period_trunk[10] = {1000, 2000, 2500, 3000, 4000, 5000, 6000, 8000,9000,10000};

int random_number(int min, int max){
	if(min==max)return min;
	return (rand()%(max - min) +min);

}

double float_rand( double min, double max )
{
    double r = rand() / (double) RAND_MAX;
    return min + r * ( max - min );
}

void generer_periodicTasks(system_T *s, int U, int nbTrans, int nbTasks, int gType, int pT,int print){
	int i, rp, ro, u_trans, u_task, cost;
	double fu_trans, fu_trans_total,fu_trans_rest, fu_task, fu_task_total,fu_task_rest;
	if(U<nbTrans){
	printf("Utilization is too small!\nUtilization can't be smaller than nbTrans\n");
	exit(1);
	}
	int u_rest=U-nbTrans;// randomly distributed, to avoid 0
	elem_trans_T *current_trans;
	fu_trans_total = U*1.0;
	for(i=0;i<nbTrans;i++){
	
		
		current_trans = create_trans();

		// choisir la period
		if(pT==0){
			rp = random_number(5000,10000);
		}else{
			rp = period_trunk[random_number(0,10)];
		}

		// distribute utilisation to each transaction
		if (gType==0){ // gType = 0, randomly distributed
			int u_r = 0;
			if(i==nbTrans-1) u_trans = 1 + u_rest;
			else{
				if(u_rest!=0){
				u_r = random_number(0, u_rest+1);
				u_rest -=u_r;
				}
			
				u_trans = 1 + u_r;
			}
		}else if(gType==1){//  equally distributed
			if(i==0) u_trans = U/nbTrans + U - U/nbTrans*nbTrans;
			else 
			u_trans = U/nbTrans;
			
		}else {
			if(i!=(nbTrans-1)){
			fu_trans_rest = fu_trans_total *pow(float_rand(0.00001,1), 1.0/(nbTrans-1-i));
			fu_trans = fu_trans_total - fu_trans_rest;
			fu_trans_total = fu_trans_rest;
			}
			else fu_trans = fu_trans_total;
		}
		int j = 0;
		int cost_total = (u_trans * rp)/100;
		int cost_rest1 =cost_total- nbTasks;

		fu_task_total = fu_trans;
		//fu_task_total = (double)((fu_task_total*rp/100.0) - nbTasks)/rp*100.0;
		while (j<nbTasks){
			// distribute utilisation to each task
			int cost_rest =cost_total- nbTasks;
			if(gType==0){ // gType = 0, randomly distributed
				int cr=0;
				if(j==(nbTasks-1)) cost = 1 +cost_rest1;
				else{
					if(cost_rest1!=0){
						cr = random_number(0,cost_rest1+1);
						cost_rest1 -=cr;
					}
					cost = 1+ cr;
				}
			}else if(gType==1){// equally distributed
				if(j==0)cost = 1 + cost_rest/nbTasks + cost_rest - cost_rest/nbTasks*nbTasks;
				else
				cost = 1 + cost_rest/nbTasks;
				
			}else{
				if(j!=(nbTasks-1)){
				fu_task_rest = fu_task_total *(pow(float_rand(0.00001,1), 1.0/(nbTasks-1-j)));
				fu_task = fu_task_total - fu_task_rest;
				fu_task_total = fu_task_rest;
				}
				else fu_task = fu_task_total;

				cost = ceil((fu_task*rp)/100.0);
			}
			// set offset randomly
			ro = random_number(0,rp);
			add_task_to_trans(current_trans, create_task(NUMBER, random_number(0,nbTrans*nbTasks), cost, rp, rp, ro));
			j++;
		}
		
		add_trans_to_system(s, *current_trans);
		free(current_trans);
	}

	if(print==0){
		printf("\n************************************************************************\n");
		printf("*                    Generate tasks successfully!                      *\n");
		printf("************************************************************************\n\n");
		printf("Utilisation: %d\n",U);
		printf("Number of transaction: %d\n", nbTrans);
		printf("Number of tasks in each transaction: %d\n", nbTasks);
	}

}



