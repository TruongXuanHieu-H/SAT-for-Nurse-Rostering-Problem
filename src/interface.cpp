#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <signal.h>
#include <stdexcept>
#include <map>

#include "global_data.h"
#include "utils/signal_handler.h"
#include "utils/usage.h"
#include "utils/version.h"
#include "enum/encode_type.h"

int get_number_arg(std::string const &arg)
{
    try
    {
        std::size_t pos;
        int x = std::stoi(arg, &pos);
        if (pos < arg.size())
        {
            std::cerr << "e [Param] Trailing characters after number: " << arg << '\n';
        }
        return x;
    }
    catch (std::invalid_argument const &ex)
    {
        std::cerr << "e [Param] Invalid number: " << arg << '\n';
        return 0;
    }
    catch (std::out_of_range const &ex)
    {
        std::cerr << "e [Param] Number out of range: " << arg << '\n';
        return 0;
    }
}

int main(int argc, char **argv)
{
    SignalHandler::init_signals();

    Version::print_version();

    if (argc < 2)
    {
        Helper::print_usage();
        return 1;
    }


    for (int i = 1; i < argc; i++)
    {
        if (i == 1)     // First args is <--help> or <number of nurses> only 
        {
            if (argv[i] == std::string("--help"))
            {
                Helper::print_usage();
                return 0;
            }
            else 
            {
                int number_of_nurses = get_number_arg(argv[i]);
                if (number_of_nurses == 0)
                {
                    std::cerr << "e [Param] Number of nurses is invalid. Stop.";
                    return 0;
                }
                GlobalData::set_number_nurses(number_of_nurses);
            }
        }
        else if (i == 2)    // Second args is <schedule period (days)> only
        {   
            int schedule_period = get_number_arg(argv[i]);
            if (schedule_period == 0)
            {
                std::cerr << "e [Param] Schedule period is invalid. Stop.";
                return 0;
            }
            GlobalData::set_schedule_period(schedule_period);
        }
        else if (argv[i] == std::string("--bdd"))
        {
            GlobalData::set_encode_type(EncodeType::BDD);
        }
        else if (argv[i] == std::string("--card"))
        {
            GlobalData::set_encode_type(EncodeType::Card);
        }
        else if (argv[i] == std::string("--pairwise"))
        {
            GlobalData::set_encode_type(EncodeType::Pairwise);
        }
        else if (argv[i] == std::string("--scl"))
        {
            GlobalData::set_encode_type(EncodeType::SCL);
        }
        else if (argv[i] == std::string("--seq"))
        {
            GlobalData::set_encode_type(EncodeType::Seq);
        }
        else if (argv[i] == std::string("--check-solution"))
        {
            GlobalData::set_enable_solution_verification(true);
        }
        else if (argv[i] == std::string("-limit-memory"))
        {
            int lim_mem = get_number_arg(argv[++i]);
            if (lim_mem <= 0)
            {
                std::cerr << "e [Param] Error, memory limit has to be positive.\n";
                return 1;
            }
            std::cout << "c [Param] Memory limit is set to " << lim_mem << ".\n";
            GlobalData::set_memory_limit(lim_mem);
        }
        else if (argv[i] == std::string("-limit-real-time"))
        {
            int limit_real_time = get_number_arg(argv[++i]);
            if (limit_real_time <= 0)
            {
                std::cerr << "e [Param] Error, real time limit has to be positive.\n";
                return 1;
            }
            std::cout << "c [Param] Real time limit is set to " << limit_real_time << ".\n";
            GlobalData::set_real_time_limit(limit_real_time);
        }
        else if (argv[i] == std::string("-limit-elapsed-time"))
        {
            int limit_elapsed_time = get_number_arg(argv[++i]);
            if (limit_elapsed_time <= 0)
            {
                std::cerr << "e [Param] Error, elapsed time limit has to be positive.\n";
                return 1;
            }
            std::cout << "c [Param] Elapsed time limit is set to " << limit_elapsed_time << ".\n";
            GlobalData::set_elapsed_time_limit(limit_elapsed_time);
        }
        else if (argv[i] == std::string("-sample-rate"))
        {
            int sample_rate = get_number_arg(argv[++i]);
            if (sample_rate <= 0)
            {
                std::cerr << "e [Param] Error, sample rate has to be positive.\n";
                return 1;
            }
            std::cout << "c [Param] Sample rate is set to " << sample_rate << ".\n";
            GlobalData::set_sample_rate(sample_rate);
        }
        else if (argv[i] == std::string("-report-rate"))
        {
            int report_rate = get_number_arg(argv[++i]);
            if (report_rate <= 0)
            {
                std::cerr << "e [Param] Error, report rate has to be positive.\n";
                return 1;
            }
            std::cout << "c [Param] Report rate is set to " << report_rate << ".\n";
            GlobalData::set_report_rate(report_rate);
        }
        else if (argv[i] == std::string("-split-size"))
        {
            int split_size = get_number_arg(argv[++i]);
            if (split_size <= 0)
            {
                std::cerr << "e [Param] Error, split size has to be positive.\n";
                return 1;
            }
            std::cout << "c [Param] Splitting clauses at length " << split_size << ".\n";
            GlobalData::set_max_cnf_size(split_size);
        }
        else
        {
            std::cerr << "e [Param] Unrecognized option: " << argv[i] << std::endl;
            return 1;
        }
    }

    return 0;
}
