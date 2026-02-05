#ifndef NRP_ENCODER_BDD_H
#define NRP_ENCODER_BDD_H

#include "nrp_encoder.h"

class NRPEncoderBDD : public NRPEncoder
{
    public:
        NRPEncoderBDD(SATSolver *sat_solver, VarHandler *var_handler);
        ~NRPEncoderBDD();

    protected:
        void encode_at_least_20_work_shifts_every_28_days() override;
        void encode_at_least_4_off_days_every_14_days() override;
        void encode_between_1_and_4_night_shifts_every_14_days() override;
        void encode_between_4_and_8_evening_shifts_every_14_days() override;
        void encode_night_shifts_cannot_appear_on_consecutive_days() override;
        void encode_between_2_and_4_evening_or_night_shifts_every_7_days() override;
        void encode_at_most_6_work_shifts_every_7_days() override;

        void update_var_handler(const Clauses& clauses);
        void update_var_handler(const Clause& clause);
};

#endif // NRP_ENCODER_SCL_H