#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H


#include "enum/encode_type.h"
#include <unordered_map>
#include <limits>

class GlobalData
{
protected:
    static EncodeType _encode_type;

    static bool _enable_solution_verification;   // Enable to verify the final solution
    // static int _max_cnf_size;                     // Maximum size of CNF clauses

    // Data
    static int _number_nurses;              // Number of nurses
    static int _schedule_period;    // Number of schedule days

    // Statistics
    static int _sample_rate; // Interval of sampler, in microseconds
    static int _report_rate; // Interval of report, in number of sampler

    // Restrictions
    static float _memory_limit;       // bound of total memory consumed by all the processes, in megabyte
    static float _real_time_limit;    // bound of time consumed by main process, in seconds
    static float _elapsed_time_limit; // bound of total time consumed by all the process, in seconds

public:
    GlobalData();
    ~GlobalData();

    static EncodeType get_encode_type();
    static bool set_encode_type(EncodeType enc_type);

    static bool get_enable_solution_verification();
    static bool set_enable_solution_verification(bool enable);
    static int get_max_cnf_size();
    static bool set_max_cnf_size(int limit);


    static int get_number_nurses();
    static bool set_number_nurses(int n);
    static int get_schedule_period();
    static bool set_schedule_period(int days);

    static int get_sample_rate();
    static bool set_sample_rate(int rate);
    static int get_report_rate();
    static bool set_report_rate(int rate);

    static float get_memory_limit();
    static bool set_memory_limit(float limit);
    static float get_real_time_limit();
    static bool set_real_time_limit(float limit);
    static float get_elapsed_time_limit();
    static bool set_elapsed_time_limit(float limit);

};

#endif // GLOBAL_DATA_H