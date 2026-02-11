#include "nrp_encoder_pairwise.h"

#include <iostream>

NRPEncoderPairwise::NRPEncoderPairwise(SATSolver *sat_solver, VarHandler *var_handler) : NRPEncoder(sat_solver, var_handler)
{
    std::cout << "c [NRPEncoderPairwise] Encoder initialized for " << number_of_nurses << " nurses over " << schedule_period << " days.\n";
}

NRPEncoderPairwise::~NRPEncoderPairwise()
{
    
}

void NRPEncoderPairwise::encode_at_least_20_work_shifts_every_28_days()
{
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
        for (int j = 0; j <= schedule_period - 28; ++j)
        {
            std::vector<int> literals(not_off_day[i].begin() + j, not_off_day[i].begin() + j + 28);
            encode_alk(literals, 20);
        }
    }
}

void NRPEncoderPairwise::encode_at_least_4_off_days_every_14_days()
{
    std::vector<std::vector<int>> off_days(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            off_days[i][j] = shift_schedule[i][j][3]; // Off shift
        }
    }

    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 14; j++)
        {
            std::vector<int> literals(off_days[i].begin() + j, off_days[i].begin() + j + 14);
            encode_alk(literals, 4);
        }
    }
}

void NRPEncoderPairwise::encode_between_1_and_4_night_shifts_every_14_days()
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
            sat_solver->add_clause(clause);
        }
    }

    // At most 4 night shifts every 14 days
    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 14; j++)
        {
            std::vector<int> literals(night_shift[i].begin() + j, night_shift[i].begin() + j + 14);
            encode_amk(literals, 4);
        }
    }
}

void NRPEncoderPairwise::encode_between_4_and_8_evening_shifts_every_14_days()
{
    std::vector<std::vector<int>> evening_shifts(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            evening_shifts[i][j] = shift_schedule[i][j][1]; // Evening shifts
        }
    }

    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 14; j++)
        {
            std::vector<int> literals(evening_shifts[i].begin() + j, evening_shifts[i].begin() + j + 14);
            encode_alk(literals, 4);
        }
    }

    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 14; j++)
        {
            std::vector<int> literals(evening_shifts[i].begin() + j, evening_shifts[i].begin() + j + 14);
            encode_amk(literals, 8);
        }
    }
}

void NRPEncoderPairwise::encode_night_shifts_cannot_appear_on_consecutive_days()
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
            sat_solver->add_clause(clause);
        }
    }
}

void NRPEncoderPairwise::encode_between_2_and_4_evening_or_night_shifts_every_7_days()
{
    std::vector<std::vector<int>> evening_or_night_shift(number_of_nurses, std::vector<int>(schedule_period * 2, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            evening_or_night_shift[i][2 * j] = shift_schedule[i][j][1]; // Day shift
            evening_or_night_shift[i][2 * j + 1] = shift_schedule[i][j][2]; // Off day
        }
    }

    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 7; j++)
        {
            std::vector<int> literals(evening_or_night_shift[i].begin() + j * 2, evening_or_night_shift[i].begin() + j * 2 + 7 * 2);
            encode_alk(literals, 2);
        }
    }

    for (int i = 0; i < number_of_nurses; i++)
    {
        for (int j = 0; j <= schedule_period - 7; j++)
        {
            std::vector<int> literals(evening_or_night_shift[i].begin() + j * 2, evening_or_night_shift[i].begin() + j * 2 + 7 * 2);
            encode_amk(literals, 4);
        }
    }
}

void NRPEncoderPairwise::encode_at_most_6_work_shifts_every_7_days()
{
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
        for (int j = 0; j <= schedule_period - 7; ++j)
        {
            std::vector<int> literals(not_off_day[i].begin() + j, not_off_day[i].begin() + j + 7);
            encode_amk(literals, 6);
        }
    }
}

void NRPEncoderPairwise::encode_amk(const std::vector<int>& literals, int k)
{
    std::vector<std::vector<int>> combinations_size_k = get_all_combinations(literals, k);
    for (auto combination : combinations_size_k)
    {
        Clause clause;
        for (int literal : combination)
        {
            clause.push_back(-literal);
        }
        sat_solver->add_clause(clause);
    }
}

void NRPEncoderPairwise::encode_alk(const std::vector<int>& literals, int k)
{
    std::vector<int> reverse_literals;
    for (int i = 0; i < (int)literals.size(); ++i)
    {
        reverse_literals.push_back(-literals[i]);
    }
    encode_amk(reverse_literals, (int)reverse_literals.size() - k);
}