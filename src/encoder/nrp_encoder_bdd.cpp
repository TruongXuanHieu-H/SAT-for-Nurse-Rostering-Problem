#include "nrp_encoder_bdd.h"

#include <iostream>
#include <pblib/pb2cnf.h>

NRPEncoderBDD::NRPEncoderBDD(SATSolver *sat_solver, VarHandler *var_handler) : NRPEncoder(sat_solver, var_handler)
{
    std::cout << "c [NRPEncoderBDD] Encoder initialized for " << number_of_nurses << " nurses over " << schedule_period << " days.\n";
}

NRPEncoderBDD::~NRPEncoderBDD()
{
    
}

void NRPEncoderBDD::encode_at_least_20_work_shifts_every_28_days()
{
}

void NRPEncoderBDD::encode_at_least_4_off_days_every_14_days()
{
}

void NRPEncoderBDD::encode_between_1_and_4_night_shifts_every_14_days()
{
}

void NRPEncoderBDD::encode_between_4_and_8_evening_shifts_every_14_days()
{
}

void NRPEncoderBDD::encode_night_shifts_cannot_appear_on_consecutive_days()
{
}

void NRPEncoderBDD::encode_between_2_and_4_evening_or_night_shifts_every_7_days()
{
}

void NRPEncoderBDD::encode_at_most_6_work_shifts_every_7_days()
{
}