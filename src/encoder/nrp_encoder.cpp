#include "nrp_encoder.h"

NRPEncoder::NRPEncoder(SATSolver *sat_solver, VarHandler *var_handler) : sat_solver(sat_solver), var_handler(var_handler)
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

NRPEncoder::~NRPEncoder()
{
}