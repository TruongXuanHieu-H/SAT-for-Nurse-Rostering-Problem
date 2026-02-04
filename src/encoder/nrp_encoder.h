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

        virtual void encode_at_most_1_shift_every_day() final 
        {
            for (int i = 0; i < number_of_nurses; ++i)
            {
                for (int j = 0; j < schedule_period; ++j)
                {
                    for (int k1 = 0; k1 < 4; ++k1)
                    {
                        for (int k2 = k1 + 1; k2 < 4; ++k2)
                        {
                            Clause amo;
                            amo.push_back(-shift_schedule[i][j][k1]);
                            amo.push_back(-shift_schedule[i][j][k2]);
                            sat_solver->add_clause(amo);
                        }
                    }

                    Clause alo;
                    alo.push_back(shift_schedule[i][j][0]); // Day
                    alo.push_back(shift_schedule[i][j][1]); // Evening
                    alo.push_back(shift_schedule[i][j][2]); // Night
                    alo.push_back(shift_schedule[i][j][3]); // Off
                    sat_solver->add_clause(alo);
                }
            }
        }
        virtual void encode_at_least_20_work_shifts_every_28_days() = 0;
        virtual void encode_at_least_4_off_days_every_14_days() = 0;
        virtual void encode_between_1_and_4_night_shifts_every_14_days() = 0;
        virtual void encode_between_4_and_8_evening_shifts_every_14_days() = 0;
        virtual void encode_night_shifts_cannot_appear_on_consecutive_days() = 0;
        virtual void encode_between_2_and_4_evening_or_night_shifts_every_7_days() = 0;
        virtual void encode_at_most_6_work_shifts_every_7_days() = 0;

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