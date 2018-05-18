/* Natalie Rubin and Ziyao Yan
 * This is the intake function.
 * This function takes in the file and parses each line for the relavent information.
 * The function then makes the appropriate system calls.
 * It also creates the jobs that are later sent into the system.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include "system.h"
#include "job.h"

std::vector<std::string> parse(std::string);
System* process_config(std::vector<std::string>);
Job* process_arrival(std::vector<std::string>);


int main(int argc, const char* argv[]){
  if(argc != 2){
    std::cout << "usage: ./intake <filename>" << std::endl;
    return 1;
  }

  std::ifstream fh(argv[1]);
  std::string line;
  System *system;
  Job *job_arrive;

  while(getline(fh, line)){
    std::vector<std::string> split_line = parse(line);
    int t, j, d;
    switch((char)line[0]){
    case 'C' :
      std::istringstream(split_line[1]) >> t;
      system = process_config(split_line);
      break;
    case 'A' :
      job_arrive = process_arrival(split_line);
      if(job_arrive->get_mem_req() <= system->get_tot_mem()){
        std::istringstream(split_line[1]) >> t;
        system->jump_to_time(t);
        system->submit(job_arrive);
      }
      else{
        std::cout << "job needs more memeory than system total" << std::endl;
      }
      break;
    case 'Q' :
      std::istringstream(split_line[1]) >> t;
      std::istringstream(split_line[2].substr(2)) >> j;
      std::istringstream(split_line[3].substr(2)) >> d;
      std::cout << "request | time: " << t 
            << " job number: " << j 
            << " devices: " << d 
            << std::endl;
      system->jump_to_time(t);
      system->request(t, j, d);
      break;
    case 'L' :
      std::istringstream(split_line[1]) >> t;
      std::istringstream(split_line[2].substr(2)) >> j;
      std::istringstream(split_line[3].substr(2)) >> d;
      std::cout << "release | time: " << t 
            << " job number: " << j 
            << " devices: " << d 
            << std::endl;
      system->jump_to_time(t);
      system->release(t, j, d);
      break;
    case 'D' :
      std::istringstream(split_line[1]) >> t;
      std::cout << "display | time: " << t<< std::endl;
      system->jump_to_time(t);
      system->status();
      if(split_line[1] == "9999"){
        //Dump the final state. TODO
        std::cout << "end of input file. Dumping final state " << std::endl;
      }
      break;
    default:
      std::cout << "invalid instruction: " << line << std::endl;
      return 1;
    }
  }

  return 0;
}

std::vector<std::string> parse(std::string input){
  std::istringstream iss(input);
  std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
                                 std::istream_iterator<std::string>());
  return results;
}

System* process_config(std::vector<std::string> split_line){
  int t, m, s, q;
  std::istringstream(split_line[1]) >> t;
  std::istringstream(split_line[2].substr(2)) >> m;
  std::istringstream(split_line[3].substr(2)) >> s;
  std::istringstream(split_line[4].substr(2)) >> q;
  std::cout << "config | time: " << t 
            << " memory: " << m  
            << " serial devices: " << s  
            << " time quantum: " << q
            << std::endl;
  return new System(t,m,s,q);
}

Job* process_arrival(std::vector<std::string> split_line){
  int t, j, m, s, r, p;
  std::istringstream(split_line[1]) >> t;
  std::istringstream(split_line[2].substr(2)) >> j;
  std::istringstream(split_line[3].substr(2)) >> m;
  std::istringstream(split_line[4].substr(2)) >> s;
  std::istringstream(split_line[5].substr(2)) >> r;
  std::istringstream(split_line[6].substr(2)) >> p;
  std::cout << "arrival | time: " << t 
            << " job number: " << j 
            << " require memory: " << m 
            << " max demand: " << s 
            << " run time: " << r 
            << " priority: " << p 
            << std::endl;
  return new Job(t,j,m,s,r,p);
}
