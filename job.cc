/* Natalie Rubin and Ziyao Yan
 * This file contains the class description 
 * for the instance of a single job.
 */

#include <iostream>
#include "job.h"

using namespace std;

Job::Job(int arr_time,
         int job_num,
         int mem_req,
         int max_dev,
         int run_time,
         int priority){
  this->arr_time = arr_time;
  this->job_num = job_num;
  this->mem_req = mem_req;
  this->max_dev = max_dev;
  this->run_time = run_time;
  this->priority = priority;
}

int Job::get_arr_time(){return this->arr_time;}
int Job::get_job_num(){return this->job_num;}
int Job::get_mem_req(){return this->mem_req;}
int Job::get_max_dev(){return this->max_dev;}
int Job::get_run_time(){return this->run_time;}
int Job::get_priority(){return this->priority;}
