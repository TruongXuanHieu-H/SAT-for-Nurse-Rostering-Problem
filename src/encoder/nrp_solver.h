#ifndef NRP_SOLVER_H
#define NRP_SOLVER_H

#include "../global_data.h"
#include <sys/types.h>

enum class LimitViolation
{
    NONE = 0,
    MEMORY = 1,
    REAL_TIME = 2,
    ELAPSED_TIME = 3
};

class NRPSolver
{
public:
    NRPSolver();
    ~NRPSolver();

    bool encode_and_solve();

private:
    // Process
    pid_t lim_pid = -1;
    pid_t nrp_pid = -1;

    // Resource usage
    float consumed_memory = 0.0f;       // MB
    float consumed_real_time = 0.0f;     // s
    float consumed_elapsed_time = 0.0f;  // s
    float* max_consumed_memory = nullptr;
    
    // Sampler
    size_t sampler_count = 0;
    
    // Helpers
    void reset_state();
    LimitViolation check_limit() const;

    void create_limit_pid();
    void create_nrp_pid();

    int do_nrp_task();

};

#endif // NRP_SOLVER_H