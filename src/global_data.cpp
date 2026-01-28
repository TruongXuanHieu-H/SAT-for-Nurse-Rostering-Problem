#include "global_data.h"

#include <iostream>

GlobalData::GlobalData() {}

GlobalData::~GlobalData() {}



EncodeType GlobalData::_encode_type = EncodeType::SCL;

bool GlobalData::_enable_solution_verification = false;
int GlobalData::_max_cnf_size = 0;

int GlobalData::_number_nurses = 0;
int GlobalData::_schedule_period = 0;

int GlobalData::_sample_rate = 100000;
int GlobalData::_report_rate = 100;

float GlobalData::_memory_limit = std::numeric_limits<float>::max();
float GlobalData::_real_time_limit = std::numeric_limits<float>::max();
float GlobalData::_elapsed_time_limit = std::numeric_limits<float>::max();

EncodeType GlobalData::get_encode_type()
{
    return GlobalData::_encode_type;
}
bool GlobalData::set_encode_type(EncodeType enc_type)
{
    GlobalData::_encode_type = enc_type;
    std::cout << "c [Config] Encoding type is set to " << encode_type_to_string(GlobalData::_encode_type) << ".\n";
    return true;
}

bool GlobalData::get_enable_solution_verification()
{
    return GlobalData::_enable_solution_verification;
}
bool GlobalData::set_enable_solution_verification(bool enable)
{
    GlobalData::_enable_solution_verification = enable;
    std::cout << "c [Config] Solution verification is " << (GlobalData::_enable_solution_verification ? "enabled" : "disabled") << ".\n";
    return true;
}
int GlobalData::get_max_cnf_size()
{
    return GlobalData::_max_cnf_size;
}
bool GlobalData::set_max_cnf_size(int limit)
{
    if (limit < 0)
    {
        std::cerr << "e [Config] Error, maximum CNF clause size has to be non-negative.\n";
        return false;
    }
    
    GlobalData::_max_cnf_size = limit;
    std::cout << "c [Config] Maximum CNF clause size is set to " << GlobalData::_max_cnf_size << ".\n";
    return true;
}


int GlobalData::get_number_nurses()
{
    return GlobalData::_number_nurses;
}
bool GlobalData::set_number_nurses(int n)
{
    if (n <= 0)
    {
        std::cerr << "e [Config] Error, number of nurses has to be positive.\n";
        return false;
    }

    GlobalData::_number_nurses = n;
    std::cout << "c [Config] Number of nurses is set to " << GlobalData::_number_nurses << ".\n";
    return true;
}
int GlobalData::get_schedule_period()
{
    return GlobalData::_schedule_period;
}
bool GlobalData::set_schedule_period(int days)
{
    if (days <= 0)
    {
        std::cerr << "e [Config] Error, schedule period has to be positive.\n";
        return false;
    }

    GlobalData::_schedule_period = days;
    std::cout << "c [Config] Schedule period is set to " << GlobalData::_schedule_period << ".\n";
    return true;
}

int GlobalData::get_sample_rate()
{
    return GlobalData::_sample_rate;
}
bool GlobalData::set_sample_rate(int rate)
{
    if (rate <= 0)
    {
        std::cerr << "e [Config] Error, sample rate has to be positive.\n";
        return false;
    }

    GlobalData::_sample_rate = rate;
    std::cout << "c [Config] Sample rate is set to " << GlobalData::_sample_rate << ".\n";
    return true;
}
int GlobalData::get_report_rate()
{
    return GlobalData::_report_rate;
}
bool GlobalData::set_report_rate(int rate)
{
    if (rate <= 0)
    {
        std::cerr << "e [Config] Error, report rate has to be positive.\n";
        return false;
    }

    GlobalData::_report_rate = rate;
    std::cout << "c [Config] Report rate is set to " << GlobalData::_report_rate << ".\n";
    return true;
}

float GlobalData::get_memory_limit()
{
    return GlobalData::_memory_limit;
}
bool GlobalData::set_memory_limit(float limit)
{
    if (limit <= 0)
    {
        std::cerr << "e [Config] Error, memory limit has to be positive.\n";
        return false;
    }

    GlobalData::_memory_limit = limit;
    std::cout << "c [Config] Memory limit is set to " << GlobalData::_memory_limit << ".\n";
    return true;
}

float GlobalData::get_real_time_limit()
{
    return GlobalData::_real_time_limit;
}
bool GlobalData::set_real_time_limit(float limit)
{
    if (limit <= 0)
    {
        std::cerr << "e [Config] Error, real time limit has to be positive.\n";
        return false;
    }

    GlobalData::_real_time_limit = limit;
    std::cout << "c [Config] Real time limit is set to " << GlobalData::_real_time_limit << ".\n";
    return true;
}

float GlobalData::get_elapsed_time_limit()
{
    return GlobalData::_elapsed_time_limit;
}
bool GlobalData::set_elapsed_time_limit(float limit)
{
    if (limit <= 0)
    {
        std::cerr << "e [Config] Error, elapsed time limit has to be positive.\n";
        return false;
    }

    GlobalData::_elapsed_time_limit = limit;
    std::cout << "c [Config] Elapsed time limit is set to " << GlobalData::_elapsed_time_limit << ".\n";
    return true;
}