/* Natalie Rubin and Ziyao Yan
 * Process class declaration
 */

#ifndef _PROCESS_H
#define _PROCESS_H

#include "job.h"

class Process{
  Job *job;
  int alloc_dev,
    elap_time,
    compl_time;

 public:
  Process(Job *job);
  void set_alloc_dev(int dev);
  void set_elap_time(int time);
  void set_compl_time(int time);
  int get_alloc_dev();
  int get_elap_time();
  int get_arr_time();
  int get_job_num();
  int get_mem_req();
  int get_max_dev();
  int get_run_time();
  int get_priority();
  int get_compl_time();
  int get_turnaround_time();
  float get_weighted_turnaround_time();
};

#endif
