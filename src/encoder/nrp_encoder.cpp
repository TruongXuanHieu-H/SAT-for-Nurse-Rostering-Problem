#include "nrp_encoder.h"

NRPEncoder::NRPEncoder(SATSolver *sat_solver, VarHandler *var_handler) : sat_solver(sat_solver), var_handler(var_handler)
{
    number_of_nurses = GlobalData::get_number_nurses();
    schedule_period = GlobalData::get_schedule_period();


    // Init shift schedule variables
    shift_schedule = new int**[number_of_nurses];
    for (int i = 0; i < number_of_nurses; ++i)
    {
        shift_schedule[i] = new int*[schedule_period];
        for (int j = 0; j < schedule_period; ++j)
        {
            shift_schedule[i][j] = new int[4]; // Day, Evening, Night, Off
            for (int k = 0; k < 4; ++k)
            {
                shift_schedule[i][j][k] = var_handler->pop_next_var(); 
            }
        }
    }
}

NRPEncoder::~NRPEncoder()
{
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            delete[] shift_schedule[i][j];
            shift_schedule[i][j] = nullptr;
        }
        delete[] shift_schedule[i];
        shift_schedule[i] = nullptr;
    }
    delete[] shift_schedule;
    shift_schedule = nullptr;
}