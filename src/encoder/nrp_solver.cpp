#include "nrp_solver.h"

#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <iostream>
#include <assert.h>
#include <chrono>
#include <cmath>
#include <pid_manager.h>
#include <global_data.h>

NRPSolver::NRPSolver()
{
    max_consumed_memory = (float *)mmap(nullptr, sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

NRPSolver::~NRPSolver()
{
}

bool NRPSolver::encode_and_solve()
{
    fflush(stdout);
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time = std::chrono::high_resolution_clock::now();
    create_limit_pid();
    create_nrp_pid();
}

bool NRPSolver::is_limit_satisfied()
{
    if (consumed_memory > GlobalData::get_memory_limit())
        return -1;

    if (consumed_real_time > GlobalData::get_real_time_limit())
        return -2;

    if (consumed_elapsed_time > GlobalData::get_elapsed_time_limit())
        return -3;

    return 0;
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
        int limit_state = is_limit_satisfied();

        while (limit_state == 0)
        {
            consumed_memory = std::round(PIDManager::get_total_memory_usage(main_pid) * 10 / 1024.0) / 10;
            consumed_real_time += std::round((float)GlobalData::get_sample_rate() * 10 / 1000000.0) / 10;
            consumed_elapsed_time += (float)(GlobalData::get_sample_rate() * (PIDManager::get_descendant_pids(main_pid).size() - 1)) / 1000000.0;

            if (consumed_memory > *max_consumed_memory)
            {
                *max_consumed_memory = consumed_memory;
                // std::cout << "[Lim] Memory consumed: " << max_consumed_memory << " MB.\n";
            }

            sampler_count++;

            if (sampler_count >= GlobalData::get_report_rate())
            {
                sampler_count = 0;
                // std::cout << "c [Lim] Sampler:\t" << "Memory: " << consumed_memory << " MB\tReal time: " << consumed_real_time << "s\tElapsed time: " << consumed_elapsed_time << "s.\n";
            }

            usleep(GlobalData::get_sample_rate());

            limit_state = is_limit_satisfied();
        }

        exit(limit_state);
    }
    else
    {
        std::cout << "c Lim pid is forked at " << lim_pid << ".\n";
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
        int result = 1;

        exit(result);
    }
    else
    {
        std::cout << "c NRP pid is forked at " << nrp_pid << ".\n";
    }
}