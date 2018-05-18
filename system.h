/* Natalie Rubin and Ziyao Yan
 * System class declaration
 */

#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <list>
#include "job.h"
#include "process.h"

class System {
  int time,
    tot_mem,
    avail_mem,
    tot_dev,
    avail_dev,
    quantum;

  std::list<Job*> *sub_q;
  std::list<Job*> *hold_q1;
  std::list<Job*> *hold_q2;

  std::list<Process*> *ready_q;
  std::list<Process*> *wait_q;
  std::list<Process*> *complete_q;
  Process* cpu;

  void swap_cpu_jobs(); // moves current process to a queue,
                        // moves next job to process

 public:
  System(int time,
         int tot_mem,
         int tot_dev,
         int quantum);
  int get_time();
  int get_tot_mem();
  int get_avail_mem();
  int get_tot_dev();
  int get_avail_dev();
  int get_quantum();
  int get_running_job_num();
  float get_avg_turnaround_time();
  float get_avg_weighted_turnaround_time();
  void set_time(int time);
  void set_avail_dev(int devices);
  void set_avail_mem(int memory);

  void submit(Job *job);
  void update();
  void status();
  void dump_json();
  void request(int time, int job_num, int dev);
  void release(int time, int job_num, int dev);
  void run_quantum(); /* move current job to back of ready queue,
                         get new job, run that job for one quantum */
  void jump_to_time(int time); // execute quantums until time t (and partials
                               // as needed)
};

#endif
