/* Natalie Rubin and Ziyao Yan
 * Test file
 */

#include <iostream>
#include <assert.h>
#include "system.h"

void system_status_test();
void system_getter_test();
void system_req_rel_test();
void system_quant_test();
void system_jump_test();

int main(){
  system_status_test();
  system_getter_test();
  system_req_rel_test();
  system_quant_test();
  system_jump_test();
  return 0;
}

void system_jump_test(){
  System *system = new System(0,15,15,2);
  system->jump_to_time(3);
  system->submit(new Job(3,1,15,15,5,1));
  system->jump_to_time(5);
  assert(system->get_running_job_num()==1);
  system->submit(new Job(5,2,10,2,1,1));
  system->jump_to_time(7);
  assert(system->get_running_job_num()==1);
  system->jump_to_time(9);
  system->status();
  assert(system->get_running_job_num()==1);
}

void system_quant_test(){
  System *system = new System(0,15,15,2);
  assert(system->get_running_job_num()==0);  
  system->run_quantum();
  assert(system->get_running_job_num()==0);
  system->submit(new Job(1,1,4,3,88,1));
  system->submit(new Job(2,2,4,4,88,1));
  system->run_quantum();
  // queue process 1, run process 1
  assert(system->get_running_job_num()==1); 
  system->run_quantum();
  // queue process 2, run it
  assert(system->get_running_job_num()==2);
}

void system_req_rel_test(){
  System *system = new System(0,15,15,2);
  system->submit(new Job(1,1,4,3,88,1));
  system->submit(new Job(2,2,16,1,1,2));
  system->request(5, 1, 4);
}

void system_status_test(){
  System *system = new System(0,15,15,2);
  system->submit(new Job(1,1,4,3,77,1));
  system->submit(new Job(2,2,16,1,1,2));
  system->submit(new Job(3,3,1,1,1,2));
  system->status();
}

void system_getter_test(){
  int time = 0;
  int mem = 15;
  int dev = 13;
  int qnt = 2;
  System *system = new System(time,mem,dev,qnt);
  assert(system->get_time()==time);
  assert(system->get_tot_mem()==mem);
  assert(system->get_tot_dev()==dev);
  assert(system->get_avail_mem()==mem);
  assert(system->get_avail_dev()==dev);
  assert(system->get_quantum()==qnt);
  time++;
  mem--;
  dev--;
  system->set_time(time);
  system->set_avail_mem(mem);
  system->set_avail_dev(dev);
  assert(system->get_avail_dev()==dev);
  assert(system->get_time()==time);
  assert(system->get_avail_mem()==mem);
  
}

