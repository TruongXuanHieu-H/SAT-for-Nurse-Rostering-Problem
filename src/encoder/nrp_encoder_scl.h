#ifndef NRP_ENCODER_SCL_H
#define NRP_ENCODER_SCL_H

#include "nrp_encoder.h"
#include <algorithm>

class NRPEncoderSCL : public NRPEncoder
{
    public:
        NRPEncoderSCL(SATSolver *sat_solver, VarHandler *var_handler);
        ~NRPEncoderSCL();
        
    protected:
        void encode_at_least_20_work_shifts_every_28_days() override;
        void encode_at_least_4_off_days_every_14_days() override;
        void encode_between_1_and_4_night_shifts_every_14_days() override;
        void encode_between_4_and_8_evening_shifts_every_14_days() override;
        void encode_night_shifts_cannot_appear_on_consecutive_days() override;
        void encode_between_2_and_4_evening_or_night_shifts_every_7_days() override;
        void encode_at_most_6_work_shifts_every_7_days() override;

        // Auxiliary variable helper
        std::unordered_map<std::string, int> aux_vars;

        std::vector<int> normalize_expression(const std::vector<int>& elements); // Sort expression elements to ensure consistent key generation

        std::string get_aux_key(const std::vector<int>& elements, int sum); // Convert expression to string. Eg. x1 + x2 + x3 >= 2 -> "x1,x2,x3,2"
        int get_aux_value(const std::string& key);                          // Get auxiliary variable for the given key (or create a new one)
        int get_aux_value(const std::vector<int>& elements, int sum);                // Get auxiliary variable for the given expression (or create a new one)

        void encode_ladder_amk_constraint(const std::vector<int>& ladder_literals, int width, int k);
        std::vector<std::vector<int>> split_windows(const std::vector<int>& ladder_literals, int width);
        void encode_window(const std::vector<int>& window_literals, int width, int k, bool is_first_window, bool is_last_window);
        void encode_connect_block(const std::vector<int>& block_literals, int width, int k); // Using 6 equations (1) -> (6)
        void encode_amk_block(const std::vector<int>& block_literals, int width, int k);     // Using all 7 equations (1) -> (7)
        void connect_blocks(const std::vector<int>& first_block, const std::vector<int>& second_block, int width, int k);

        std::vector<int> get_reversed_vector(const std::vector<int>& v)
        {
            return {v.rbegin(), v.rend()};
        }

        std::vector<int> get_first_n_elements(const std::vector<int>& v, int n)
        {
            return std::vector<int>(v.begin(), v.begin() + n);
        }

        void print_aux_vars()
        {
            std::vector<std::pair<std::string, int>> items(
                aux_vars.begin(), aux_vars.end()
            );

            std::sort(items.begin(), items.end(),
                    [](const auto& a, const auto& b) {
                        return a.second < b.second;  // sort by VALUE
                    });

            for (const auto& [key, value] : items)
            {
                std::cout << key << " -> " << value << "\n";
            }
        }
};

#endif // NRP_ENCODER_SCL_H