/* Natalie Rubin and Ziyao Yan
 * Process class implementation
 */

#include "process.h"
#include "job.h"

Process::Process(Job *job){
  this->job = job;
  this->alloc_dev = 0;
  this->elap_time = 0;
  this->compl_time = -1;
}

void Process::set_alloc_dev(int dev){this->alloc_dev = dev;}
void Process::set_elap_time(int time){this->elap_time = time;}
void Process::set_compl_time(int time){this->compl_time = time;}
int Process::get_alloc_dev(){return this->alloc_dev;}
int Process::get_elap_time(){return this->elap_time;}
int Process::get_arr_time(){return this->job->get_arr_time();}
int Process::get_job_num(){return this->job->get_job_num();}
int Process::get_mem_req(){return this->job->get_mem_req();}
int Process::get_max_dev(){return this->job->get_max_dev();}
int Process::get_run_time(){return this->job->get_run_time();}
int Process::get_priority(){return this->job->get_priority();}
int Process::get_compl_time(){return this->compl_time;}
int Process::get_turnaround_time(){
  return this->compl_time == -1  
    ? -1  // if job is not complete return -1
    : this->get_compl_time() - this->get_arr_time();
}
float Process::get_weighted_turnaround_time(){
  return this->compl_time == -1
    ? -1 // if job is not complete return -1
    : (float)this->get_turnaround_time() / (float)this->get_run_time();
}

