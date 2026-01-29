#include "nrp_encoder_scl.h"

#include <iostream>
#include "sat_solver.h"
#include "var_handler.h"

NRPEncoderSCL::NRPEncoderSCL(SATSolver *sat_solver, VarHandler *var_handler) : NRPEncoder(sat_solver, var_handler)
{
    std::cout << "c [NRPEncoderSCL] Encoder initialized for " << number_of_nurses << " nurses over " << schedule_period << " days.\n";
}

NRPEncoderSCL::~NRPEncoderSCL()
{
    std::cout << "c [NRPEncoderSCL] Encoder destroyed.\n";
}

void NRPEncoderSCL::encode_instance()
{
    // Implementation of SCL encoding goes here   

    // Encode daily shift assignment constraints (nurse works at exactly one shift per day, D, E, N, O)
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