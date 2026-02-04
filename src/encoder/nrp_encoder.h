#ifndef NRP_ENCODER_H
#define NRP_ENCODER_H

#include "sat_solver.h"
#include "../global_data.h"
#include "var_handler.h"
#include <iostream>

class NRPEncoder
{
    public:

        virtual ~NRPEncoder();
        virtual void encode_instance() = 0;

    protected:
        NRPEncoder(SATSolver *sat_solver, VarHandler *var_handler);

        SATSolver *sat_solver;
        VarHandler *var_handler;
        
        int number_of_nurses;
        int schedule_period;
        std::vector<std::vector<std::vector<int>>> shift_schedule; // shift_schedule[nurse][day][shift]



        void print_vector(const std::vector<int>& vec)
        {
            for (const auto& val : vec)
            {
                std::cout << val << " ";
            }
        }

        bool is_print_clause = false;
        void print_clause(const Clause& clause)
        {
            (void)clause;
            if (!is_print_clause)
                return;

            print_vector(clause);
            std::cout << "0\n";
        }
};

#endif // NRP_ENCODER_H