#include "nrp_solver.h"
#include "nrp_encoder_scl.h"
#include "nrp_encoder_bdd.h"
#include "../utils/pid_manager.h"
#include "../global_data.h"
#include "sat_solver_cadical.h"
#include "var_handler.h"

#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#include <iostream>
#include <cmath>
#include <chrono>

using Clock = std::chrono::steady_clock;

NRPSolver::NRPSolver()
{
    max_consumed_memory = static_cast<float*>(mmap(nullptr, sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    if (max_consumed_memory == MAP_FAILED)
    {
        std::cerr << "e Error in mmap for max_consumed_memory.\n";
        exit(1);
    }

    reset_state();
}

NRPSolver::~NRPSolver()
{
    munmap(max_consumed_memory, sizeof(float));
}

void NRPSolver::reset_state()
{
    consumed_memory = 0.0f;
    consumed_real_time = 0.0f;
    consumed_elapsed_time = 0.0f;
    sampler_count = 0;
    *max_consumed_memory = 0.0f;
}

LimitViolation NRPSolver::check_limit() const
{
    if (consumed_memory > GlobalData::get_memory_limit())
        return LimitViolation::MEMORY;

    if (consumed_real_time > GlobalData::get_real_time_limit())
        return LimitViolation::REAL_TIME;

    if (consumed_elapsed_time > GlobalData::get_elapsed_time_limit())
        return LimitViolation::ELAPSED_TIME;

    return LimitViolation::NONE;
}

bool NRPSolver::encode_and_solve()
{
    reset_state();
    fflush(stdout);

    auto wall_start = Clock::now();

    create_limit_pid();
    create_nrp_pid();

    bool limit_violated = false;
    int nrp_status = 0;
    int lim_status = 0;

    while(true)
    {
        pid_t finished_pid = waitpid(-1, nullptr, WNOHANG);
        if (finished_pid == 0)
        {
            // No child exited yet
            usleep(1000); // 1ms
            continue;
        }
        else if (finished_pid < 0)
        {
            // Error occurred
            std::cerr << "e Error in waitpid.\n";
            break;
        }
        else if (finished_pid == lim_pid)
        {
            waitpid(lim_pid, &lim_status, 0);
            limit_violated = true;

            std::cout << "c [Lim] Limit violated. Killing NRP.\n";

            kill(nrp_pid, SIGTERM);
            usleep(10000);
            kill(nrp_pid, SIGKILL);
            waitpid(nrp_pid, &nrp_status, 0);
            break;
        } 
        else if (finished_pid == nrp_pid)
        {
            waitpid(nrp_pid, &nrp_status, 0);

            std::cout << "c [NRPSolver] Solver finished. Killing Lim.\n";

            kill(lim_pid, SIGTERM);
            usleep(10000);
            kill(lim_pid, SIGKILL);
            waitpid(lim_pid, &lim_status, 0);
            break;
        }
    }

    auto wall_end = Clock::now();
    double wall_time = std::chrono::duration<double>(wall_end - wall_start).count();

    // Report
    std::cout << "c [NRPSolver] STATISTICS =========================\n";
    std::cout << "c [NRPSolver] Peak memory:     " << *max_consumed_memory << " MB\n";
    std::cout << "c [NRPSolver] Wall clock time: " << wall_time << " s\n";
    std::cout << "c [NRPSolver] ====================================\n";

    if (limit_violated)
        return false;

    return WIFEXITED(nrp_status) && WEXITSTATUS(nrp_status) == 0;
}

void NRPSolver::create_limit_pid()
{
    lim_pid = fork();
    if (lim_pid < 0)
    {
        std::cerr << "e [Lim] Fork Failed!\n";
        exit(-1);
    }
    else if (lim_pid == 0)
    {
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        std::cout << "c [Lim] Start task in PID: " << getpid() << ".\n";

        pid_t main_pid = getppid();
        auto last_sample = Clock::now();

        while (true)
        {
            auto now = Clock::now();
            float delta = std::chrono::duration<float>(now - last_sample).count();
            last_sample = now;
            consumed_real_time += delta;

            size_t mem_kb = PIDManager::get_total_memory_usage(main_pid);
            consumed_memory = std::round(mem_kb / 102.4f) / 10.0f;

            size_t proc_count = PIDManager::get_descendant_pids(main_pid).size() - 1;
            consumed_elapsed_time += delta * proc_count;

            if (consumed_memory > *max_consumed_memory)
                *max_consumed_memory = consumed_memory;

            auto limit_checker = check_limit();
            if (limit_checker != LimitViolation::NONE)
                exit(static_cast<int>(limit_checker));

            usleep(GlobalData::get_sample_rate());
        }
    }
    else
    {
        std::cout << "c [NRPSolver] Lim pid is forked at " << lim_pid << ".\n";
    }
}

void NRPSolver::create_nrp_pid()
{
    nrp_pid = fork();
    if (nrp_pid < 0)
    {
        std::cerr << "e [NRPSolver] Fork failed!\n";
        exit(-1);
    }
    else if (nrp_pid == 0)
    {
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        std::cout << "c [NRPSolver] Start task in PID: " << getpid() << ".\n";

        // Child process: perform the task
        int result = do_nrp_task();

        exit(result);
    }
    else
    {
        std::cout << "c [NRPSolver] NRP pid is forked at " << nrp_pid << ".\n";
    }
}

int NRPSolver::do_nrp_task()
{
    var_handler = new VarHandler(1);
    sat_solver = new SATSolverCadical(var_handler);

    switch (GlobalData::get_encode_type())
    {
        case EncodeType::BDD:
            nrp_encoder = new NRPEncoderBDD(sat_solver, var_handler);
            break;
        case EncodeType::Card:
            break;
        case EncodeType::Pairwise:
            break;
        case EncodeType::SCL:
            nrp_encoder = new NRPEncoderSCL(sat_solver, var_handler);
            break;
        case EncodeType::Seq:
            break;
        default:
            std::cerr << "e [NRPSolver] Unsupported encoding type.\n";
            return -1;
    }

    nrp_encoder->encode_instance();
    int sat_result = sat_solver->solve();
    if (sat_result == 10)
    {
        std::vector<std::vector<std::vector<bool>>> schedule = sat_solver->extract_result();
        if(!verify_nrp_solution(schedule))
        {
            std::cerr << "e [NRPSolver] Result is incorrect.\n";
            exit(-1);
        }
        else
        {
            int number_of_nurse = GlobalData::get_number_nurses();
            int schedule_period = GlobalData::get_schedule_period();
            std::cout << "r [NRPSolver] RESULT =============================\n";
            for (int i = 0; i < number_of_nurse; i++)
            {
                std::cout << "r [NRPSolver] Schedule for nurse " << i << ":\t";
                for (int j = 0; j < schedule_period; j++)
                {
                    if (schedule[i][j][0])
                    {
                        std::cout << "D ";
                    }
                    else if (schedule[i][j][1])
                    {
                        std::cout << "E "; 
                    }
                    else if (schedule[i][j][2])
                    {
                        std::cout << "N "; 
                    }
                    else if (schedule[i][j][3])
                    {
                        std::cout << "O "; 
                    }
                    else
                    {
                        std::cout << "\ne [NRPSolver] Error occured in schedule reporting...\n";
                        exit(-1);
                    }
                }
                std::cout << "\n";
            }
            std::cout << "r [NRPSolver] ====================================\n";
        }
    }
    else if (sat_result == 20)
    {
        std::cout << "r [NRPSolver] No feasible schedule found.\n";
    }
    else
    {
        std::cout << "e [NRPSolver] Unknown error occured for SAT solver...\n";
    }
    

    if (nrp_encoder)
    {
        delete nrp_encoder;
        nrp_encoder = nullptr;
    }
    if (sat_solver) 
    {
        delete sat_solver;
        sat_solver = nullptr;
    }
    if (var_handler)
    {
        delete var_handler;
        var_handler = nullptr;
    }

    return sat_result;
}

bool NRPSolver::verify_nrp_solution(const std::vector<std::vector<std::vector<bool>>>& schedule)
{
    std::cout << "c [NRPSolver] RESULT VERIFICATION =============================\n";

    if (!verify_at_least_20_work_shifts_every_28_days(schedule))
    {
        std::cerr << "e [NRPSolver] Verification failed: at least 20 work shifts every 28 days.\n";
        return false;
    }
    else
    {
        std::cout << "c [NRPSolver] Verification passed: at least 20 work shifts every 28 days.\n";
    }

    if (!verify_at_least_4_off_days_every_14_days(schedule))
    {
        std::cerr << "e [NRPSolver] Verification failed: at least 4 off days every 14 days.\n";
        return false;
    }
    else
    {
        std::cout << "c [NRPSolver] Verification passed: at least 4 off days every 14 days.\n";
    }

    if (!verify_between_1_and_4_night_shifts_every_14_days(schedule))
    {
        std::cerr << "e [NRPSolver] Verification failed: between 1 and 4 night shifts every 14 days.\n";
        return false;
    }
    else 
    {
        std::cout << "c [NRPSolver] Verification passed: between 1 and 4 night shifts every 14 days.\n";
    }

    if (!verify_between_4_and_8_evening_shifts_every_14_days(schedule))
    {
        std::cerr << "e [NRPSolver] Verification failed: between 4 and 8 evening shifts every 14 days.\n";
        return false;
    }
    else 
    {
        std::cout << "c [NRPSolver] Verification passed: between 4 and 8 evening shifts every 14 days.\n";
    }

    if (!verify_night_shifts_cannot_appear_on_consecutive_days(schedule))
    {
        std::cerr << "e [NRPSolver] Verification failed: night shifts cannot appear on consecutive days.\n";
        return false;
    }
    else
    {
        std::cout << "c [NRPSolver] Verification passed: night shifts cannot appear on consecutive days.\n";
    }

    if (!verify_between_2_and_4_evening_or_night_shifts_every_7_days(schedule))
    {
        std::cerr << "e [NRPSolver] Verification failed: between 2 and 4 evening or night shifts every 7 days.\n";
        return false;
    }
    else
    {
        std::cout << "c [NRPSolver] Verification passed: between 2 and 4 evening or night shifts every 7 days.\n";
    }

    if (!verify_at_most_6_work_shifts_every_7_days(schedule))
    {
        std::cerr << "e [NRPSolver] Verification failed: at most 6 work shifts every 7 days.\n";
        return false;
    }
    else
    {
        std::cout << "c [NRPSolver] Verification passed: at most 6 work shifts every 7 days.\n";
    }

    std::cout << "c [NRPSolver] All verifications passed.\n";
    std::cout << "c [NRPSolver] =================================================\n";

    return true;
}
bool NRPSolver::verify_at_least_20_work_shifts_every_28_days(const std::vector<std::vector<std::vector<bool>>>& schedule)
{
    int number_of_nurses = GlobalData::get_number_nurses();
    int schedule_period = GlobalData::get_schedule_period();

    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j <= schedule_period - 28; ++j)
        {
            int work_shift_count = 0;
            for (int d = j; d < j + 28; ++d)
            {
                if (schedule[i][d][0] || schedule[i][d][1] || schedule[i][d][2]) // Day, Evening, Night
                    work_shift_count++;
            }
            if (work_shift_count < 20)
            {
                return false;
            }
        }
    }
    return true;
}
bool NRPSolver::verify_at_least_4_off_days_every_14_days(const std::vector<std::vector<std::vector<bool>>>& schedule)
{
    int number_of_nurses = GlobalData::get_number_nurses();
    int schedule_period = GlobalData::get_schedule_period();

    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j <= schedule_period - 14; ++j)
        {
            int off_day_count_by_work_shifts = 0;
            int off_day_count_by_off_shifts = 0;
            for (int d = j; d < j + 14; ++d)
            {
                if (!schedule[i][d][0] && !schedule[i][d][1] && !schedule[i][d][2]) // No shift on this day
                    off_day_count_by_work_shifts++;
                if (schedule[i][d][3]) // Off shift
                    off_day_count_by_off_shifts++;
            }
            if (off_day_count_by_work_shifts < 4 || off_day_count_by_off_shifts < 4)
            {
                // std::cout << "Debug: Nurse " << i << ", Days " << j << " to " << (j + 13) << ": off_day_count_by_work_shifts = " << off_day_count_by_work_shifts << ", off_day_count_by_off_shifts = " << off_day_count_by_off_shifts << "\n";
                return false;
            }
        }
    }
    return true;
}
bool NRPSolver::verify_between_1_and_4_night_shifts_every_14_days(const std::vector<std::vector<std::vector<bool>>>& schedule)
{
    int number_of_nurses = GlobalData::get_number_nurses();
    int schedule_period = GlobalData::get_schedule_period();

    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 14; ++j)
        {
            int night_shifts_count = 0;
            for (int d = j; d < j + 14; ++d)
            {
                if (schedule[i][d][2]) // Night shift
                    night_shifts_count++;
            }

            if (night_shifts_count < 1 || night_shifts_count > 4)
            {
                return false;
            }
        }
    }
    return true;
}
bool NRPSolver::verify_between_4_and_8_evening_shifts_every_14_days(const std::vector<std::vector<std::vector<bool>>>& schedule)
{
    int number_of_nurses = GlobalData::get_number_nurses();
    int schedule_period = GlobalData::get_schedule_period();

    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 14; ++j)
        {
            int evening_shift_count = 0;
            for (int d = j; d < j + 14; ++d)
            {
                if (schedule[i][d][1]) // Evening shift
                    evening_shift_count++;
            }

            if (evening_shift_count < 4 || evening_shift_count > 8)
            {
                return false;
            }
        }
    }
    return true;
}
bool NRPSolver::verify_night_shifts_cannot_appear_on_consecutive_days(const std::vector<std::vector<std::vector<bool>>>& schedule)
{
    int number_of_nurses = GlobalData::get_number_nurses();
    int schedule_period = GlobalData::get_schedule_period();

    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 2; ++j)
        {
            int night_shift_count = 0;
            for (int d = j; d < j + 2 ; ++d)
            {
                if (schedule[i][d][2]) // Evening shift
                    night_shift_count++;
            }

            if (night_shift_count >= 2)
            {
                return false;
            }
        }
    }
    return true;
}
bool NRPSolver::verify_between_2_and_4_evening_or_night_shifts_every_7_days(const std::vector<std::vector<std::vector<bool>>>& schedule)
{
    int number_of_nurses = GlobalData::get_number_nurses();
    int schedule_period = GlobalData::get_schedule_period();

    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 7; ++j)
        {
            int e_or_n_shifts_count = 0;
            for (int d = j; d < j + 7 ; ++d)
            {
                if (schedule[i][d][1]) // Evening shift
                    e_or_n_shifts_count++;
                if (schedule[i][d][2]) // Night shift
                    e_or_n_shifts_count++;
            }

            if (e_or_n_shifts_count < 2 || e_or_n_shifts_count > 4)
            {
                return false;
            }
        }
    }
    return true;
}
bool NRPSolver::verify_at_most_6_work_shifts_every_7_days(const std::vector<std::vector<std::vector<bool>>>& schedule)
{
    int number_of_nurses = GlobalData::get_number_nurses();
    int schedule_period = GlobalData::get_schedule_period();

    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 7; ++j)
        {
            int work_shift_count = 0;
            for (int d = j; d < j + 7 ; ++d)
            {
                if (schedule[i][d][0]) // Day shift
                    work_shift_count++;
                if (schedule[i][d][1]) // Evening shift
                    work_shift_count++;
                if (schedule[i][d][2]) // Night shift
                    work_shift_count++;
            }

            if (work_shift_count > 6)
            {
                return false;
            }
        }
    }
    return true;
}