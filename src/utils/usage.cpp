#include <map>
#include <string>
#include <iostream>
#include <iomanip>

#include "usage.h"

const std::map<std::string, std::string> Helper::option_list = {
    {"--help", "Print usage message with all possible options"},
    {"--ladder", "Use ladder encoding [default: true]"},
    {"--check-solution", "Calculate the cyclic antibandwidth of the found SAT solution and compare it to the actual width [default: false]"},
    {"--from-ub", "Start solving with width = UB, decreasing in each iteration [default: false]"},
    {"--from-lb", "Start solving with width = LB, increasing in each iteration [default: true]"},
    {"--bin-search", "Start solving with LB+UB/2 and update LB or UB according to SAT/UNSAT result and repeat"},
    {"-split-size <n>", "Maximal allowed length of clauses, every longer clause is split up into two by introducing a new variable"},
    {"-set-lb <new LB>", "Overwrite predefined LB with <new LB>, has to be at least 2"},
    {"-set-ub <new UB>", "Overwrite predefined UB with <new UB>, has to be positive"},
};

void Helper::print_usage()
{
    std::cout << "c [Usage] Usage: abw_enc path_to_graph_file/graph_file.mtx.rnd [ <option> ... ].\n";
    std::cout << "c [Usage] where '<option>' is one of the following options:\n";
    std::cout << std::endl;
    for (const auto &option : option_list)
    {
        std::cout << std::left << "\t" << std::setw(30) << option.first << "\t" << option.second << ".\n";
    }
    std::cout << std::endl;
}