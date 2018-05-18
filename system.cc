/* Natalie Rubin and Ziyao Yan
 * System class implementation
 */

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iomanip>
#include <string>
#include "system.h"

System::System(int time,
               int tot_mem,
               int tot_dev,
               int quantum){
  this->time = time;
  this->tot_mem = tot_mem;
  this->avail_mem = tot_mem;
  this->tot_dev = tot_dev;
  this->avail_dev = tot_dev;
  this->quantum = quantum;

  this->sub_q = new std::list<Job*>();
  this->hold_q1 = new std::list<Job*>();
  this->hold_q2 = new std::list<Job*>();
  this->ready_q = new std::list<Process*>();
  this->wait_q = new std::list<Process*>();
  this->complete_q = new std::list<Process*>();

  this->cpu = NULL;
}

int System::get_time(){return this->time;}
int System::get_tot_mem(){return this->tot_mem;}
int System::get_avail_mem(){return this->avail_mem;}
int System::get_tot_dev(){return this->tot_dev;}
int System::get_avail_dev(){return this->avail_dev;}
int System::get_quantum(){return this->quantum;}
int System::get_running_job_num(){return this->cpu==NULL?0:this->cpu->get_job_num();}
float System::get_avg_turnaround_time(){
  int sum, n = 0;
  std::list<Process*>::iterator it;
  for(it = this->complete_q->begin();it != this->complete_q->end(); it++){
    sum += (*it)->get_turnaround_time();
    n++;
  }
  return n < 1
             ? -1
             : (float)sum / (float)n;
}
float System::get_avg_weighted_turnaround_time(){
  float sum, n = 0;
  std::list<Process*>::iterator it;
  for(it = this->complete_q->begin();it != this->complete_q->end(); it++){
    sum += (*it)->get_weighted_turnaround_time();
    n++;
  }
  return n < 1
             ? -1
             : sum / n;
}
void System::set_time(int time){this->time = time;}
void System::set_avail_mem(int memory){this->avail_mem = memory;}
void System::set_avail_dev(int devices){this->avail_dev = devices;}

bool sort_hold_q1(Job *job1, Job *job2){
  // sorting function for hold queue 1
  if(job1->get_run_time() == job2->get_run_time()){
    return job1->get_arr_time() < job2->get_arr_time();  // FIFO
  } else {
    return job1->get_run_time() < job2->get_run_time();  // SJF
  }
}

bool sort_hold_q2(Job *job1, Job *job2){
  // sorting function for hold queue 2
  return job1->get_arr_time() < job2->get_arr_time();  // FIFO
}

void System::submit(Job *job){
  if(job->get_mem_req() > this->get_tot_mem() ||
     job->get_max_dev() > this->get_tot_dev()){
    std::cout << "job rejected: insufficient system resources" << std::endl;
  } else if(job->get_mem_req() > this->get_avail_mem()) {
    switch(job->get_priority()){
    case 1:
      this->hold_q1->push_back(job);
      this->hold_q1->sort(sort_hold_q1);
      break;
    case 2:
      this->hold_q2->push_back(job);
      this->hold_q2->sort(sort_hold_q2);
      break;
    }
  } else {
    this->ready_q->push_back(new Process(job));
    this->set_avail_mem(this->get_avail_mem() - job->get_mem_req());
    // subtract available memory from system if adding to ready queue
  }
}

void System::jump_to_time(int time){
  if(time <= this->get_time()){
    return;
  }
  int num_quantums = (int) ((time - this->get_time()) / this->get_quantum());
  // integer division
  for(int i=0; i<num_quantums; i++){
    // run quantums until we're almost to time t
    this->run_quantum();
  }

  if(this->cpu == NULL){
    // no jobs queued
    this->set_time(time);
  } else {
    this->cpu->set_elap_time(this->cpu->get_elap_time() + time - this->get_time());
    // run partial quantum
    this->set_time(time);
  }
}

void System::run_quantum(){
  // swap next job onto the cpu then run for one quantum
  // queue up next job
  this->swap_cpu_jobs();
  if(this->cpu == NULL){
      //std::cout << "no available jobs to be run" << std::endl;
    this->set_time(this->get_time() + this->get_quantum());
    return;
  }
  if(this->cpu->get_elap_time() + this->get_quantum() < this->cpu->get_run_time()){
    // if this job does not complete in the current quantum
    this->set_time(this->get_time() + this->get_quantum());
    this->cpu->set_elap_time(this->cpu->get_elap_time()+this->get_quantum());
    // increase run time
  } else {
    this->set_time(this->get_time() +
                   (this->cpu->get_run_time() - this->cpu->get_elap_time()));
    this->cpu->set_elap_time(this->cpu->get_run_time());
  }
}

void System::swap_cpu_jobs(){
  /* this function updates which job is being run on the cpu
   * it does not increment the timer
   */
  if(this->cpu != NULL){
    // if process is done
    if(this->cpu->get_elap_time()>=this->cpu->get_run_time()){  
      // return devices
      this->set_avail_dev(this->cpu->get_alloc_dev() + this->get_avail_dev());
      this->cpu->set_alloc_dev(0);
      // return memory
      this->set_avail_mem(this->cpu->get_mem_req() + this->get_avail_mem());
      // set process completion time
      this->cpu->set_compl_time(this->get_time());
      this->complete_q->push_back(this->cpu); // add process to complete queue
    } else {
      // otherwise
      this->ready_q->push_back(this->cpu); // add process to back of ready queue
    }
  }
  // move next job to cpu
  if(!this->ready_q->empty()){
    this->cpu=this->ready_q->front();
    this->ready_q->pop_front();
  } else {
    this->cpu=NULL;
  }
}

void System::request(int time, int job_num, int dev){
  std::cout << "request at " << time << " by " << job_num << " for " << dev << " devices" << std::endl;
  if(this->cpu != NULL && this->cpu->get_job_num() == job_num){
    if(this->get_avail_dev() < dev){
      std::cout << "cannot allocate devices, not enough resources" << std::endl;
    } else {
      this->set_avail_dev(this->get_avail_dev()-dev);
      this->cpu->set_alloc_dev(this->cpu->get_alloc_dev()+dev);
    }
  } else if (this->cpu == NULL){
    std::cout << "no current running job" << std::endl;
  } else {
    std::cout << "running job does not match request" << std::endl;
  }
}

void System::release(int time, int job_num, int dev){
  std::cout << "release at " << time << " by " << job_num << " of " << dev << " devices" << std::endl;
  if(this->cpu != NULL && this->cpu->get_job_num() == job_num){
    this->set_avail_dev(this->get_avail_dev()+dev);
    this->cpu->set_alloc_dev(this->cpu->get_alloc_dev()-dev);
  } else if (this->cpu == NULL){
    std::cout << "no current running job" << std::endl;
  } else {
    std::cout << "running job does not match request" << std::endl;
  }
}


//GO NO FURTHER
void System::status(){

  std::cout << "---------- System Information ----------"
            << std::endl;
  std::cout << "Time | Tot Mem | Avail Mem | Tot Dev | Avail Dev | Quantum | CPU Job #"
            << std::endl;
  std::cout << std::setw(5)
            << this->get_time()
            << "|";
  std::cout << std::setw(9)
            << this->get_tot_mem()
            << "|";
  std::cout << std::setw(11)
            << this->get_avail_mem()
            << "|";
  std::cout << std::setw(9)
            << this->get_tot_dev()
            << "|";
  std::cout << std::setw(11)
            << this->get_avail_dev()
            << "|";
  std::cout << std::setw(9)
            << this->get_quantum()
            << "|";
  std::cout << std::setw(10)
            << (this->cpu == NULL ? 0 : this->cpu->get_job_num());
                // 0 if cpu empty
  std::cout << std::endl << std::endl;
  std::cout << std::setw(30)
            << "avg turnaround time: "
            << this->get_avg_turnaround_time()
            << std::endl;
  std::cout << std::setw(30)
            << "avg weighted turnaround time: "
            << this->get_avg_weighted_turnaround_time()
            << std::endl << std::endl;


  std::cout << "------------- CPU ----------------------"
            << std::endl;
  std::cout << "Job # | Arr | Mem | Exectime | MDev | Runtime | Pri | ADev "
            << std::endl;
  if(this->cpu != NULL){
    std::cout << std::setw(6)
              << this->cpu->get_job_num()
              << "|";
    std::cout << std::setw(5)
              << this->cpu->get_arr_time()
              << "|";
    std::cout << std::setw(5)
              << this->cpu->get_mem_req()
              << "|";
    std::cout << std::setw(10)
              << this->cpu->get_elap_time()
              << "|";
    std::cout << std::setw(6)
              << this->cpu->get_max_dev()
              << "|";
    std::cout << std::setw(9)
              << this->cpu->get_run_time()
              << "|";
    std::cout << std::setw(5)
              << this->cpu->get_priority()
              << "|";
    std::cout << std::setw(6)
              << this->cpu->get_alloc_dev()
              << std::endl;
  }
  std::cout << std::endl;

  std::cout << "------------- Hold Queue 1 -------------"
            << std::endl;
  std::cout << "Job # | Arr | Mem | Dev | Run | Pri "
            << std::endl;
  std::list<Job*>::iterator it1;
  for(it1=hold_q1->begin();it1!=hold_q1->end();it1++){
    std::cout << std::setw(6)
              << (*it1)->get_job_num()
              << "|";
    std::cout << std::setw(5)
              << (*it1)->get_arr_time()
              << "|";
    std::cout << std::setw(5)
              << (*it1)->get_mem_req()
              << "|";
    std::cout << std::setw(5)
              << (*it1)->get_max_dev()
              << "|";
    std::cout << std::setw(5)
              << (*it1)->get_run_time()
              << "|";
    std::cout << std::setw(5)
              << (*it1)->get_priority()
              << std::endl;
  }
  std::cout << std::endl;


  std::cout << "------------- Hold Queue 2 -------------"
            << std::endl;
  std::cout << "Job # | Arr | Mem | Dev | Run | Pri "
            << std::endl;
  for(it1=hold_q2->begin();it1!=hold_q2->end();it1++){
    std::cout << std::setw(6)
              << (*it1)->get_job_num()
              << "|";
    std::cout << std::setw(5)
              << (*it1)->get_arr_time()
              << "|";
    std::cout << std::setw(5)
              << (*it1)->get_mem_req()
              << "|";
    std::cout << std::setw(5)
              << (*it1)->get_max_dev()
              << "|";
    std::cout << std::setw(5)
              << (*it1)->get_run_time()
              << "|";
    std::cout << std::setw(5)
              << (*it1)->get_priority()
              << std::endl;
  }
  std::cout << std::endl;

  std::cout << "------------- Ready Queue --------------"
            << std::endl;
  std::cout << "Job # | Arr | Mem | Exectime | MDev | Runtime | Pri | ADev "
            << std::endl;
  std::list<Process*>::iterator it2;
  for(it2=ready_q->begin();it2!=ready_q->end();it2++){
    std::cout << std::setw(6)
              << (*it2)->get_job_num()
              << "|";
    std::cout << std::setw(5)
              << (*it2)->get_arr_time()
              << "|";
    std::cout << std::setw(5)
              << (*it2)->get_mem_req()
              << "|";
    std::cout << std::setw(10)
              << (*it2)->get_elap_time()
              << "|";
    std::cout << std::setw(6)
              << (*it2)->get_max_dev()
              << "|";
    std::cout << std::setw(9)
              << (*it2)->get_run_time()
              << "|";
    std::cout << std::setw(5)
              << (*it2)->get_priority()
              << "|";
    std::cout << std::setw(6)
              << (*it2)->get_alloc_dev()
              << std::endl;
  }
  std::cout << std::endl;

  std::cout << "------------- Wait Queue ---------------"
            << std::endl;
  
  std::cout << "Job # | Arr | Mem | Exectime | MDev | Runtime | Pri | ADev "
            << std::endl;
  for(it2=wait_q->begin();it2!=wait_q->end();it2++){
    std::cout << std::setw(6)
              << (*it2)->get_job_num()
              << "|";
    std::cout << std::setw(5)
              << (*it2)->get_arr_time()
              << "|";
    std::cout << std::setw(5)
              << (*it2)->get_mem_req()
              << "|";
    std::cout << std::setw(10)
              << (*it2)->get_elap_time()
              << "|";
    std::cout << std::setw(6)
              << (*it2)->get_max_dev()
              << "|";
    std::cout << std::setw(9)
              << (*it2)->get_run_time()
              << "|";
    std::cout << std::setw(5)
              << (*it2)->get_priority()
              << "|";
    std::cout << std::setw(6)
              << (*it2)->get_alloc_dev()
              << std::endl;
  }
  std::cout << std::endl;
  
  std::cout << "------------- Complete Queue -----------"
            << std::endl;
  std::cout << "Job # | Arr | Mem | MDev | Runtime | Pri | Compl Time | TAT | WTAT"
            << std::endl;

  for(it2=complete_q->begin();it2!=complete_q->end();it2++){

    std::cout << std::setw(6)
              << (*it2)->get_job_num()
              << "|";
    std::cout << std::setw(5)
              << (*it2)->get_arr_time()
              << "|";
    std::cout << std::setw(5)
              << (*it2)->get_mem_req()
              << "|";
    std::cout << std::setw(6)
              << (*it2)->get_max_dev()
              << "|";
    std::cout << std::setw(9)
              << (*it2)->get_run_time()
              << "|";
    std::cout << std::setw(5)
              << (*it2)->get_priority()
              << "|";
    std::cout << std::setw(12)
              << (*it2)->get_compl_time()
              << "|";
    std::cout << std::setw(5)
              << (*it2)->get_turnaround_time()
              << "|";
    std::cout << std::setw(6)
              << (*it2)->get_weighted_turnaround_time()
              << std::endl;
  }
  std::cout << std::endl;

  this->dump_json();
  
}

void System::dump_json(){

  std::list<Job*>::iterator it1;
  std::list<Process*>::iterator it2;

  std::ofstream fh;

  std::string get_time_string;
  std::ostringstream convert;
  convert << this->get_time();
  get_time_string.append("D");
  get_time_string.append(convert.str());
  get_time_string.append(".json");
  fh.open(get_time_string.c_str(), std::ofstream::out | std::ofstream::trunc);

  fh << "{" << std::endl;
  fh << "  \"current_time\": " << (this->get_time())
     << "," << std::endl;
  fh << "  \"total_memory\": " << (this->get_tot_mem())
     << "," << std::endl;
  fh << "  \"available_memory\": " << (this->get_avail_mem())
     << "," << std::endl;
  fh << "  \"quantum\": " << (this->get_quantum())
     << "," << std::endl;
  fh << "  \"avg_turnaround\": " << (this->get_avg_turnaround_time())
     << "," << std::endl;
  fh << "  \"avg_weighted_turnaround\": "
     << (this->get_avg_weighted_turnaround_time())
     << "," << std::endl;

  fh << "  \"readyq\": [" << std::endl;
  for(it2=this->ready_q->begin();it2!=this->ready_q->end();it2++){
    fh << "    " << ((*it2)->get_job_num()) << "," << std::endl;
  }
  fh << "  ]," << std::endl;

  fh << "  \"running\": " << (this->cpu==NULL?0:this->cpu->get_job_num())
     << "," << std::endl;

  fh << "  \"holdq2\": [" << std::endl;
  for(it1=this->hold_q2->begin();it1!=this->hold_q2->end();it1++){
    fh << "    " << ((*it1)->get_job_num()) << "," << std::endl;
  }
  fh << "  ]," << std::endl;

  fh << "  \"holdq1\": [" << std::endl;
  for(it1=this->hold_q1->begin();it1!=this->hold_q1->end();it1++){
    fh << "    " << ((*it1)->get_job_num()) << "," << std::endl;
  }
  fh << "  ]," << std::endl;

  fh << "  \"completeq\": [" << std::endl;
  for(it2=this->complete_q->begin();it2!=this->complete_q->end();it2++){
    fh << "    " << ((*it2)->get_job_num()) << "," << std::endl;
  }
  fh << "  ]," << std::endl;
  
  fh << "  \"waitq\": [" << std::endl;
  for(it2=this->wait_q->begin();it2!=this->wait_q->end();it2++){
    fh << "    " << ((*it2)->get_job_num()) << "," << std::endl;
  }
  fh << "  ]," << std::endl;

  
  fh << "  \"job\": [" << std::endl;
  
  for(it1=this->hold_q1->begin();it1!=this->hold_q1->end();it1++){
    fh << "    {" << std::endl;
    fh << "      \"arrival_time\": " << (*it1)->get_arr_time()
       << std::endl;
    fh << "      \"job_num\": " << (*it1)->get_job_num()
       << std::endl;
    fh << "      \"memory_required\": " << (*it1)->get_mem_req()
       << std::endl;
    fh << "      \"max_devices\": " << (*it1)->get_max_dev()
       << std::endl;
    fh << "      \"run_time\": " << (*it1)->get_run_time()
       << std::endl;
    fh << "      \"priority\": " << (*it1)->get_priority()
       << std::endl;
    fh << "    }," << std::endl;
  }

  for(it1=this->hold_q2->begin();it1!=this->hold_q2->end();it1++){
    fh << "    {" << std::endl;
    fh << "      \"arrival_time\": " << (*it1)->get_arr_time()
       << std::endl;
    fh << "      \"job_num\": " << (*it1)->get_job_num()
       << std::endl;
    fh << "      \"memory_required\": " << (*it1)->get_mem_req()
       << std::endl;
    fh << "      \"max_devices\": " << (*it1)->get_max_dev()
       << std::endl;
    fh << "      \"run_time\": " << (*it1)->get_run_time()
       << std::endl;
    fh << "      \"priority\": " << (*it1)->get_priority()
       << std::endl;
    fh << "    }," << std::endl;
  }
  
  for(it2=this->ready_q->begin();it2!=this->ready_q->end();it2++){
    fh << "    {" << std::endl;
    fh << "      \"arrival_time\": " << (*it2)->get_arr_time()
       << std::endl;
    fh << "      \"job_num\": " << (*it2)->get_job_num()
       << std::endl;
    fh << "      \"memory_required\": " << (*it2)->get_mem_req()
       << std::endl;
    fh << "      \"max_devices\": " << (*it2)->get_max_dev()
       << std::endl;
    fh << "      \"run_time\": " << (*it2)->get_run_time()
       << std::endl;
    fh << "      \"priority\": " << (*it2)->get_priority()
       << std::endl;
    fh << "      \"allocated_devices\": " << (*it2)->get_alloc_dev()
       << std::endl;
    fh << "      \"elapsed_time\": " << (*it2)->get_elap_time()
       << std::endl;
    fh << "    }," << std::endl;
  }

  for(it2=this->wait_q->begin();it2!=this->wait_q->end();it2++){
    fh << "    {" << std::endl;
    fh << "      \"arrival_time\": " << (*it2)->get_arr_time()
       << std::endl;
    fh << "      \"job_num\": " << (*it2)->get_job_num()
       << std::endl;
    fh << "      \"memory_required\": " << (*it2)->get_mem_req()
       << std::endl;
    fh << "      \"max_devices\": " << (*it2)->get_max_dev()
       << std::endl;
    fh << "      \"run_time\": " << (*it2)->get_run_time()
       << std::endl;
    fh << "      \"priority\": " << (*it2)->get_priority()
       << std::endl;
    fh << "      \"allocated_devices\": " << (*it2)->get_alloc_dev()
       << std::endl;
    fh << "      \"elapsed_time\": " << (*it2)->get_elap_time()
       << std::endl;
    fh << "    }," << std::endl;
  }

  for(it2=this->complete_q->begin();it2!=this->complete_q->end();it2++){
    fh << "    {" << std::endl;
    fh << "      \"arrival_time\": " << (*it2)->get_arr_time()
       << std::endl;
    fh << "      \"job_num\": " << (*it2)->get_job_num()
       << std::endl;
    fh << "      \"memory_required\": " << (*it2)->get_mem_req()
       << std::endl;
    fh << "      \"max_devices\": " << (*it2)->get_max_dev()
       << std::endl;
    fh << "      \"run_time\": " << (*it2)->get_run_time()
       << std::endl;
    fh << "      \"priority\": " << (*it2)->get_priority()
       << std::endl;
    fh << "      \"allocated_devices\": " << (*it2)->get_alloc_dev()
       << std::endl;
    fh << "      \"elapsed_time\": " << (*it2)->get_elap_time()
       << std::endl;
    fh << "      \"turnaround_time\": " << (*it2)->get_turnaround_time()
       << std::endl;
    fh << "      \"weighted_turnaround_time\": "
       << (*it2)->get_weighted_turnaround_time()
       << std::endl;
    fh << "    }," << std::endl;
  }

  if(this->cpu!=NULL){
    fh << "    {" << std::endl;
    fh << "      \"arrival_time\": " << this->cpu->get_arr_time()
       << std::endl;
    fh << "      \"job_num\": " << this->cpu->get_job_num()
       << std::endl;
    fh << "      \"memory_required\": " << this->cpu->get_mem_req()
       << std::endl;
    fh << "      \"max_devices\": " << this->cpu->get_max_dev()
       << std::endl;
    fh << "      \"run_time\": " << this->cpu->get_run_time()
       << std::endl;
    fh << "      \"priority\": " << this->cpu->get_priority()
       << std::endl;
    fh << "      \"allocated_devices\": " << this->cpu->get_alloc_dev()
       << std::endl;
    fh << "      \"elapsed_time\": " << this->cpu->get_elap_time()
       << std::endl;
    fh << "    }," << std::endl;
  }

  fh << "  ]," << std::endl;
  
  fh << "}" << std::endl;
  
}   
    
    
    
    
