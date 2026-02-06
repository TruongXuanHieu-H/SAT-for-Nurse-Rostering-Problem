#ifndef NRP_ENCODER_PAIRWISE_H
#define NRP_ENCODER_PAIRWISE_H

#include "nrp_encoder.h"

class NRPEncoderPairwise: public NRPEncoder
{
    public:
        NRPEncoderPairwise(SATSolver *sat_solver, VarHandler *var_handler);
        ~NRPEncoderPairwise();

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

        void dfs_combinations(const std::vector<int>& elements, int start, int need, std::vector<int>& cur, std::vector<std::vector<int>>& res)
        {
            if (need == 0)
            {
                res.push_back(cur);
                return;
            }

            for (int i = start; i <= (int)elements.size() - need; ++i)
            {
                cur.push_back(elements[i]);
                dfs_combinations(elements, i + 1, need - 1, cur, res);
                cur.pop_back();
            }
        }

        std::vector<std::vector<int>> get_all_combinations(const std::vector<int>& elements, int combination_size)
        {
            std::vector<std::vector<int>> res;
            std::vector<int> cur;

            if (combination_size < 1 || combination_size > (int)elements.size())
                return res;

            dfs_combinations(elements, 0, combination_size, cur, res);
            return res;
        }
};

#endif // NRP_ENCODER_PAIRWISE_H