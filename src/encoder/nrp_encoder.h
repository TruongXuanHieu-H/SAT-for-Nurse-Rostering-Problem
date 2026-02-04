#ifndef NRP_ENCODER_H
#define NRP_ENCODER_H

#include "sat_solver.h"
#include "../global_data.h"
#include "var_handler.h"
#include <iostream>

class NRPEncoder
{
    public:

        virtual ~NRPEncoder() {};
        virtual void encode_instance() = 0;

    protected:
        NRPEncoder(SATSolver *sat_solver, VarHandler *var_handler) : sat_solver(sat_solver), var_handler(var_handler) 
        {
            number_of_nurses = GlobalData::get_number_nurses();
            schedule_period = GlobalData::get_schedule_period();


            // Init shift schedule variables
            for (int i = 0; i < number_of_nurses; ++i)
            {
                for (int j = 0; j < schedule_period; ++j)
                {
                    for (int k = 0; k < 4; ++k)
                    {
                        shift_schedule[i][j][k] = var_handler->pop_next_var(); 
                    }
                }
            }
        }

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