#ifndef NRP_SOLVER_H
#define NRP_SOLVER_H

#include "global_data.h"

class NRPSolver
{
public:
    NRPSolver();
    ~NRPSolver();

    bool encode_and_solve();

private:

    pid_t nrp_pid;
    pid_t lim_pid;

    float *max_consumed_memory;
    float consumed_memory = 0;       // total memory consumed by all the processes, in megabyte
    float consumed_real_time = 0;    // time consumed by main process, in seconds
    float consumed_elapsed_time = 0; // total time consumed by all the process, in seconds

    int sampler_count = 0;
    
    void create_limit_pid();
    void create_nrp_pid();
    bool is_limit_satisfied();

};

#endif // NRP_SOLVER_H