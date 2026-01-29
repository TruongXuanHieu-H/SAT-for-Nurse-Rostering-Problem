#include "nrp_solver.h"
#include "nrp_encoder_scl.h"
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

            std::cout << "c [NRP] Solver finished. Killing Lim.\n";

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
    std::cout << "c ========= NRP SOLVER REPORT =========\n";
    std::cout << "c Peak memory:     " << *max_consumed_memory << " MB\n";
    std::cout << "c Real time:       " << consumed_real_time << " s\n";
    std::cout << "c Elapsed time:    " << consumed_elapsed_time << " s\n";
    std::cout << "c Wall clock time: " << wall_time << " s\n";
    std::cout << "c ====================================\n";

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
        std::cout << "c [Main] Lim pid is forked at " << lim_pid << ".\n";
    }
}

void NRPSolver::create_nrp_pid()
{
    nrp_pid = fork();
    if (nrp_pid < 0)
    {
        std::cerr << "e [NRP] Fork failed!\n";
        exit(-1);
    }
    else if (nrp_pid == 0)
    {
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        std::cout << "c [NRP] Start task in PID: " << getpid() << ".\n";

        // Child process: perform the task
        int result = do_nrp_task();

        exit(result);
    }
    else
    {
        std::cout << "c [Main] NRP pid is forked at " << nrp_pid << ".\n";
    }
}

int NRPSolver::do_nrp_task()
{
    sat_solver = new SATSolverCadical();
    var_handler = new VarHandler(1);

    switch (GlobalData::get_encode_type())
    {
        case EncodeType::BDD:
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
            std::cerr << "e [NRP] Unsupported encoding type.\n";
            return -1;
    }

    nrp_encoder->encode_instance();

    // Child process: perform the task
    int result = 0;

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

    exit(result);
}