#ifndef NRP_ENCODER_SCL_H
#define NRP_ENCODER_SCL_H

#include "nrp_encoder.h"

class NRPEncoderSCL : public NRPEncoder
{
    public:
        NRPEncoderSCL(SATSolver *sat_solver, VarHandler *var_handler);
        ~NRPEncoderSCL();

        void encode_instance() override;
};

#endif // NRP_ENCODER_SCL_H