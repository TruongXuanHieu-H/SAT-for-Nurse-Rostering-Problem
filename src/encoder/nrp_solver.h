#ifndef NRP_SOLVER_H
#define NRP_SOLVER_H

#include "../global_data.h"
#include <sys/types.h>
#include "nrp_encoder.h"
#include "sat_solver.h"
#include "var_handler.h"

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
    // Encoder
    NRPEncoder *nrp_encoder;
    SATSolver *sat_solver;
    VarHandler *var_handler;

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


    bool verify_nrp_solution();
    bool verify_at_least_20_work_shifts_every_28_days(const std::vector<std::vector<std::vector<bool>>>& schedule);
    bool verify_at_least_4_off_days_every_14_days(const std::vector<std::vector<std::vector<bool>>>& schedule);
    bool verify_between_1_and_4_night_shifts_every_14_days(const std::vector<std::vector<std::vector<bool>>>& schedule);
    bool verify_between_4_and_8_evening_shifts_every_14_days(const std::vector<std::vector<std::vector<bool>>>& schedule);
    bool verify_night_shifts_cannot_appear_on_consecutive_days(const std::vector<std::vector<std::vector<bool>>>& schedule);
    bool verify_between_2_and_4_evening_or_night_shifts_every_7_days(const std::vector<std::vector<std::vector<bool>>>& schedule);
    bool verify_at_most_6_work_shifts_every_7_days(const std::vector<std::vector<std::vector<bool>>>& schedule);


};

#endif // NRP_SOLVER_H