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
        int ***shift_schedule; // shift_schedule[nurse][day][shift]
};

#endif // NRP_ENCODER_H