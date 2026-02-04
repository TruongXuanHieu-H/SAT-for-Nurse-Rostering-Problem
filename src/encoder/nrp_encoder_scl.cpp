#include "nrp_encoder_scl.h"

#include <iostream>
#include <algorithm>
#include "sat_solver.h"
#include "var_handler.h"
#include <numeric> 

NRPEncoderSCL::NRPEncoderSCL(SATSolver *sat_solver, VarHandler *var_handler) : NRPEncoder(sat_solver, var_handler)
{
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                aux_vars[std::to_string(shift_schedule[i][j][k]) + ",1"] = shift_schedule[i][j][k]; // Prepopulate aux_vars with basic shift variables
                aux_vars[std::to_string(-shift_schedule[i][j][k]) + ",1"] = -shift_schedule[i][j][k];
            }
        }
    }

    std::cout << "c [NRPEncoderSCL] Encoder initialized for " << number_of_nurses << " nurses over " << schedule_period << " days.\n";
}

NRPEncoderSCL::~NRPEncoderSCL()
{
    
}

std::vector<int> NRPEncoderSCL::normalize_expression(const std::vector<int>& elements)
{
    std::vector<int> sorted_clause = elements;
    std::sort(sorted_clause.begin(), sorted_clause.end());
    sorted_clause.erase(std::unique(sorted_clause.begin(), sorted_clause.end()), sorted_clause.end());
    if (elements.size() != sorted_clause.size())
    {
        std::cerr << "w [NRPEncoderSCL] Warning: Duplicate literals found in expression during normalization ";
        print_vector(elements);
        std::cout << "\n";
        exit(-1);
    }
    return sorted_clause;
}

std::string NRPEncoderSCL::get_aux_key(const std::vector<int>& values, int sum)
{
    std::vector<int> normalized_expression = normalize_expression(values);
    std::string key;
    for (size_t i = 0; i < normalized_expression.size(); ++i)
    {
        key += std::to_string(normalized_expression[i]);
        key += ",";
    }
    key += std::to_string(sum);
    return key;
}

int NRPEncoderSCL::get_aux_value(const std::string& key)
{
    auto it = aux_vars.find(key);
    if (it != aux_vars.end())
    {
        return it->second;
    }
    else
    {
        int new_var = var_handler->pop_next_var();
        aux_vars[key] = new_var;
        return new_var;
    }
}

int NRPEncoderSCL::get_aux_value(const std::vector<int>& values, int sum)
{
    std::string key = get_aux_key(values, sum);
    return get_aux_value(key);
}

void NRPEncoderSCL::encode_instance()
{
    std::cout << "c [NRPEncoderSCL] Start encoding.\n";
    encode_at_most_1_shift_every_day();
    encode_at_least_20_work_shifts_every_28_days();
    encode_at_least_4_off_days_every_14_days();
    encode_between_1_and_4_night_shifts_every_14_days();
    encode_between_4_and_8_evening_shifts_every_14_days();
    encode_night_shifts_cannot_appear_on_consecutive_days();
    encode_between_2_and_4_evening_or_night_shifts_every_7_days();
    encode_at_most_6_work_shifts_every_7_days();
    std::cout << "c [NRPEncodeSCL] Finish encoding.\n";
}

void NRPEncoderSCL::encode_at_most_1_shift_every_day()
{
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

void NRPEncoderSCL::encode_at_least_20_work_shifts_every_28_days()
{
    // At most 8 off days every 28 days -> at least 20 work shifts every 28 days

    std::vector<std::vector<int>> off_days(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            off_days[i][j] = shift_schedule[i][j][3]; // Off shift
        }
    }

    for (int i = 0; i < number_of_nurses; ++i)
    {
        encode_ladder_amk_constraint(off_days[i], 28, 8);
    }
}

void NRPEncoderSCL::encode_at_least_4_off_days_every_14_days()
{
    // At most 10 work days every 14 days -> at least 4 off days every 14 days
    std::vector<std::vector<int>> work_days(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            work_days[i][j] = -shift_schedule[i][j][3]; // Work day = - off day
        }
    }

    for (int i = 0; i < number_of_nurses; ++i)
    {
        encode_ladder_amk_constraint(work_days[i], 14, 10);
    }
}

void NRPEncoderSCL::encode_between_1_and_4_night_shifts_every_14_days()
{
    // At least 1 night shift every 14 days - Using pairwise encoding
    std::vector<std::vector<int>> night_shift(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            night_shift[i][j] = shift_schedule[i][j][2]; // Night shift
        }
    }

    for (int i = 0; i < number_of_nurses; i++) 
    {
        for (int j = 0; j <= schedule_period - 14; j++)
        {
            Clause clause;
            for (int m = 0; m < 14; m++) 
            {
                clause.push_back(night_shift[i][j + m]);
            }
            print_clause(clause);
            sat_solver->add_clause(clause);
        }
    }

    // At most 4 night shifts every 14 days
    for (int i = 0; i < number_of_nurses; ++i)
    {
        encode_ladder_amk_constraint(night_shift[i], 14, 4);
    }
}

void NRPEncoderSCL::encode_between_4_and_8_evening_shifts_every_14_days()
{
    // At most 10 non-evening shifts every 14 days -> at least 4 evening shifts every 14 days
    std::vector<std::vector<int>> non_evening_shifts(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            non_evening_shifts[i][j] = -shift_schedule[i][j][1]; // Non-evening shifts
        }
    }

    for (int i = 0; i < number_of_nurses; ++i)
    {
        encode_ladder_amk_constraint(non_evening_shifts[i], 14, 10);
    }

    // At most 8 evening shifts every 14 days
    std::vector<std::vector<int>> evening_shifts(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            evening_shifts[i][j] = shift_schedule[i][j][1]; // Evening shifts
        }
    }


    for (int i = 0; i < number_of_nurses; ++i)
    {
        encode_ladder_amk_constraint(evening_shifts[i], 14, 8);
    }
}

void NRPEncoderSCL::encode_night_shifts_cannot_appear_on_consecutive_days()
{
    std::vector<std::vector<int>> night_shift(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            night_shift[i][j] = shift_schedule[i][j][2]; // Night shift
        }
    }

    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 2; j++)
        {
            Clause clause;
            clause.push_back(-night_shift[i][j]);
            clause.push_back(-night_shift[i][j+1]);
            print_clause(clause);
            sat_solver->add_clause(clause);
        }
    }
}

void NRPEncoderSCL::encode_between_2_and_4_evening_or_night_shifts_every_7_days()
{
    // Implementation goes here
    // At most 5 day shifts or day offs every 7 days -> At least 2 evening or night shifts every 7 days
    std::vector<std::vector<int>> day_shift_or_off_day(number_of_nurses, std::vector<int>(schedule_period * 2, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            day_shift_or_off_day[i][2 * j] = shift_schedule[i][j][0]; // Day shift
            day_shift_or_off_day[i][2 * j + 1] = shift_schedule[i][j][3]; // Off day
        }
    }

    for (int i = 0; i < number_of_nurses; ++i)
    {
        encode_ladder_amk_constraint(day_shift_or_off_day[i], 14, 5);
    }

    // At most 4 eveing or night shifts every 7 days
    std::vector<std::vector<int>> e_or_n_shift(number_of_nurses, std::vector<int>(schedule_period * 2, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            e_or_n_shift[i][2 * j] = shift_schedule[i][j][1]; // Evening shift
            e_or_n_shift[i][2 * j + 1] = shift_schedule[i][j][2]; // Night shift
        }
    }

    for (int i = 0; i < number_of_nurses; ++i)
    {
        encode_ladder_amk_constraint(e_or_n_shift[i], 14, 4);
    }
}

void NRPEncoderSCL::encode_at_most_6_work_shifts_every_7_days()
{
    // Implementation goes here
    std::vector<std::vector<int>> not_off_day(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            not_off_day[i][j] = -shift_schedule[i][j][3]; // Not off day
        }
    }

    for (int i = 0; i < number_of_nurses; ++i) 
    {
        encode_ladder_amk_constraint(not_off_day[i], 7, 6);
    }
}

void NRPEncoderSCL::encode_ladder_amk_constraint(const std::vector<int>& ladder_literals, int width, int k)
{
    std::vector<std::vector<int>> windows = split_windows(ladder_literals, width);

    for (int i = 0; i < (int)windows.size(); ++i)
    {
        bool is_first_window = (i == 0);
        bool is_last_window = (i == (int)windows.size() - 1);
        encode_window(windows[i], width, k, is_first_window, is_last_window);
    }

    for (int i = 0; i < (int)windows.size() - 1; ++i)
    {
        connect_blocks(get_reversed_vector(windows[i]), windows[i + 1], width, k);
    }
}

std::vector<std::vector<int>> NRPEncoderSCL::split_windows(const std::vector<int>& ladder_literals, int width)
{
    std::vector<std::vector<int>> windows;
    for (int i = 0; i < (int)ladder_literals.size(); i += width)
    {
        std::vector<int> window;
        for (int j = 0; j < width && (i + j) < (int)ladder_literals.size(); ++j)
        {
            window.push_back(ladder_literals[i + j]);
        }
        windows.push_back(window);
    }
    return windows;
}

void NRPEncoderSCL::encode_window(const std::vector<int>& window_literals, int width, int k, bool is_first_window, bool is_last_window)
{
    // Implementation of window encoding goes here
    if (is_first_window)
    {
        // Handle first window specifics
        if ((int)window_literals.size() != width)
        {
            std::cerr << "[NRPEncoderSCL] First window has invalid width.\n";
            exit(-1);
        }

        std::vector<int> amk_block_literals = get_reversed_vector(window_literals);
        encode_amk_block(amk_block_literals, width, k);
    }
    else if (is_last_window)
    {
        // Handle last window specifics
        std::vector<int> amk_block_literals = window_literals;
        encode_amk_block(amk_block_literals, width, k);
    }
    else
    {
        // Handle middle windows specifics
        if ((int)window_literals.size() != width)
        {
            std::cerr << "[NRPEncoderSCL] Middle window has invalid width.\n";
            exit(-1);
        }

        std::vector<int> amk_block_literals = window_literals;
        encode_amk_block(amk_block_literals, width, k);
        std::vector<int> connect_block_literals = get_reversed_vector(window_literals);
        encode_connect_block(connect_block_literals, width, k);
    }
}

void NRPEncoderSCL::encode_connect_block(const std::vector<int>& block_literals, int width, int k)
{
    int w = (int)block_literals.size();

    if (w != width)
    {
        std::cerr << "[NRPEncoderSCL] Connect block has invalid width.\n";
        exit(-1);
    }
    

    // Encode equation (1)
    for (int j = 2; j <= w - 1; ++j)
    {
        Clause clause;
        clause.push_back(-block_literals[j - 1]);
        clause.push_back(get_aux_value(get_first_n_elements(block_literals, j), 1));
        print_clause(clause);
        sat_solver->add_clause(clause);
    }

    // Encode equation (2)
    for (int j = 2; j <= w - 1; j++){
        for (int s = 1; s <= std::min(j - 1, k); s++)
        {
            Clause clause;
            clause.push_back(-get_aux_value(get_first_n_elements(block_literals, j - 1), s));
            clause.push_back(get_aux_value(get_first_n_elements(block_literals, j), s));
            print_clause(clause);
            sat_solver->add_clause(clause);
        }
    }

    // Encode equation (3)
    for (int j = 2; j <= w - 1; j++){
        for (int s = 2; s <= std::min(j, k); s++)
        {
            Clause clause;
            clause.push_back(-block_literals[j - 1]);
            clause.push_back(-get_aux_value(get_first_n_elements(block_literals, j - 1), s - 1));
            clause.push_back(get_aux_value(get_first_n_elements(block_literals, j), s));
            print_clause(clause);
            sat_solver->add_clause(clause);
        }
    }

    // Encode equation (4)
    for (int j = 2; j <= k; ++j)
    {
        Clause clause;
        clause.push_back(block_literals[j - 1]);
        clause.push_back(-get_aux_value(get_first_n_elements(block_literals, j), j));
        print_clause(clause);
        sat_solver->add_clause(clause);
    }


    // Encode equation (5)
    for (int j = 2; j <= w - 1; j++){
        for (int s = 2; s <= std::min(j, k); s++)
        {
            Clause clause;
            clause.push_back(get_aux_value(get_first_n_elements(block_literals, j - 1), s - 1));
            clause.push_back(-get_aux_value(get_first_n_elements(block_literals, j), s));
            print_clause(clause);
            sat_solver->add_clause(clause);
        }
    }

    // Encode equation (6)
    for (int j = 2; j <= w - 1; j++){
        for (int s = 1; s <= std::min(j - 1, k); s++)
        {
            Clause clause;
            clause.push_back(block_literals[j - 1]);
            clause.push_back(get_aux_value(get_first_n_elements(block_literals, j - 1), s));
            clause.push_back(-get_aux_value(get_first_n_elements(block_literals, j), s));
            print_clause(clause);
            sat_solver->add_clause(clause);
        }
    }
}

void NRPEncoderSCL::encode_amk_block(const std::vector<int>& block_literals, int width, int k)
{ 
    int w = (int)block_literals.size();

    if (w == width)
    {
        // Encode equations (1) to (6)
        encode_connect_block(block_literals, width, k);
    } 
    else 
    {
        // Encode equation (1)
        for (int j = 2; j <= w; ++j)
        {
            Clause clause;
            clause.push_back(-block_literals[j - 1]);
            clause.push_back(get_aux_value(get_first_n_elements(block_literals, j), 1));
            print_clause(clause);
            sat_solver->add_clause(clause);
        }

        // Encode equation (2)
        for (int j = 2; j <= w; j++){
            for (int s = 1; s <= std::min(j - 1, k); s++)
            {
                Clause clause;
                clause.push_back(-get_aux_value(get_first_n_elements(block_literals, j - 1), s));
                clause.push_back(get_aux_value(get_first_n_elements(block_literals, j), s));
                print_clause(clause);
                sat_solver->add_clause(clause);
            }
        }

        // Encode equation (3)
        for (int j = 2; j <= w; j++){
            for (int s = 2; s <= std::min(j, k); s++)
            {
                Clause clause;
                clause.push_back(-block_literals[j - 1]);
                clause.push_back(-get_aux_value(get_first_n_elements(block_literals, j - 1), s - 1));
                clause.push_back(get_aux_value(get_first_n_elements(block_literals, j), s));
                print_clause(clause);
                sat_solver->add_clause(clause);
            }
        }

        // Encode equation (4)
        for (int j = 2; j <= k && j <= w; ++j)
        {
            Clause clause;
            clause.push_back(block_literals[j - 1]);
            clause.push_back(-get_aux_value(get_first_n_elements(block_literals, j), j));
            print_clause(clause);
            sat_solver->add_clause(clause);
        }

        // Encode equation (5)
        for (int j = 2; j <= w; j++){
            for (int s = 2; s <= std::min(j, k); s++)
            {
                Clause clause;
                clause.push_back(get_aux_value(get_first_n_elements(block_literals, j - 1), s - 1));
                clause.push_back(-get_aux_value(get_first_n_elements(block_literals, j), s));
                print_clause(clause);
                sat_solver->add_clause(clause);
            }
        }

        // Encode equation (6)
        for (int j = 2; j <= w; j++){
            for (int s = 1; s <= std::min(j - 1, k); s++)
            {
                Clause clause;
                clause.push_back(block_literals[j - 1]);
                clause.push_back(get_aux_value(get_first_n_elements(block_literals, j - 1), s));
                clause.push_back(-get_aux_value(get_first_n_elements(block_literals, j), s));
                print_clause(clause);
                sat_solver->add_clause(clause);
            }
        }
    }

    // Encode equation (7) 
    for (int j = k + 1; j <= w; ++j)
    {
        Clause clause;
        clause.push_back(-block_literals[j - 1]);
        clause.push_back(-get_aux_value(get_first_n_elements(block_literals, j - 1), k));
        print_clause(clause);
        sat_solver->add_clause(clause);
    }
}

void NRPEncoderSCL::connect_blocks(const std::vector<int>& first_block, const std::vector<int>& second_block, int width, int k)
{
    for (int i = 1; i <= (int)second_block.size() && i <= (width - 1); ++i)
    {
        auto first_expression = get_first_n_elements(first_block, (int)first_block.size() - i);
        auto second_expression = get_first_n_elements(second_block, i);

        for (int j = 1; j <= k; j++)
        {
            int bound_left = k - j + 1;
            int bound_right = j;
            if ((int)first_expression.size() <= bound_left - 1 || (int)second_expression.size() <= bound_right - 1)
            {
                continue;
            }
            
            Clause clause;
            clause.push_back(-get_aux_value(first_expression, bound_left));
            clause.push_back(-get_aux_value(second_expression, bound_right));

            sat_solver->add_clause(clause);
            print_clause(clause);
        }
    }
}