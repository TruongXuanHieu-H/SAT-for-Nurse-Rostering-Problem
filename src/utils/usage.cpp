#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

#include "usage.h"

const std::vector<std::pair<std::string, std::string>> Helper::option_list = {
    // Encoding selection (choose one)
    {"--bdd",        "Use BDD encoding"},
    {"--card",       "Use cardinality encoding"},
    {"--pairwise",   "Use pairwise encoding"},
    {"--scl",        "Use SCL encoding"},
    {"--seq",        "Use sequential counter encoding"},

    // Verification
    {"--check-solution",
     "Verify the final SAT solution against the actual objective [default: false]"},

    // Limits
    {"-limit-memory <MB>",
     "Limit total memory usage in megabytes (must be positive)"},
    {"-limit-real-time <sec>",
     "Limit real time of main process in seconds (must be positive)"},
    {"-limit-elapsed-time <sec>",
     "Limit total elapsed time of all processes in seconds (must be positive)"},

    // Statistics
    {"-sample-rate <microseconds>",
     "Create a sample every <microseconds> microseconds (must be positive)"},
    {"-report-rate <samples>",
     "Print a log every <samples> samples (must be positive)"},

    // CNF handling
    {"-split-size <n>",
     "Split clauses longer than <n> literals. Set to 0 to disable splitting (must be non-negative)"}
};


void Helper::print_usage()
{
    std::cout << "c [Usage] Usage: nrp_enc <number_of_nurses> <schedule_period> [ <option> ... ].\n";
    std::cout << "c [Usage] where '<option>' is one of the following options:\n";
    std::cout << std::endl;
    for (const auto &option : option_list)
    {
        std::cout << std::left << "\t" << std::setw(30) << option.first << "\t" << option.second << ".\n";
    }
    std::cout << std::endl;
}