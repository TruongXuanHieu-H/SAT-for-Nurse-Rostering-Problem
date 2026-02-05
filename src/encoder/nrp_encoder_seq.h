#ifndef NRP_ENCODER_SEQ_H
#define NRP_ENCODER_SEQ_H

#include "nrp_encoder.h"
#include <sstream>
#include <regex>

class NRPEncoderSeq: public NRPEncoder
{
    public:
        NRPEncoderSeq(SATSolver *sat_solver, VarHandler *var_handler);
        ~NRPEncoderSeq();

    protected:
        void encode_at_least_20_work_shifts_every_28_days() override;
        void encode_at_least_4_off_days_every_14_days() override;
        void encode_between_1_and_4_night_shifts_every_14_days() override;
        void encode_between_4_and_8_evening_shifts_every_14_days() override;
        void encode_night_shifts_cannot_appear_on_consecutive_days() override;
        void encode_between_2_and_4_evening_or_night_shifts_every_7_days() override;
        void encode_at_most_6_work_shifts_every_7_days() override;

        void encode_amk(const std::vector<int>& literals, int k);
        void encode_alk(const std::vector<int>& literals, int k);
};

#endif // NRP_ENCODER_SEQ_H