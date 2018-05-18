/* Natalie Rubin and Ziyao Yan
 * Job class declaration
 */

#ifndef _JOB_H
#define _JOB_H

class Job {
  int arr_time,
    job_num,
    mem_req,
    max_dev,
    run_time,
    priority;
  
 public:
  Job(int arr_time,
      int job_num,
      int mem_req,
      int max_dev,
      int run_time,
      int priority);
  int get_arr_time();
  int get_job_num();
  int get_mem_req();
  int get_max_dev();
  int get_run_time();
  int get_priority();
};


#endif
