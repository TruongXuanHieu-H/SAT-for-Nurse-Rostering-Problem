#include "nrp_encoder_card.h"

#include <iostream>
#include <pblib/pb2cnf.h>
#include <pblib/PBConfig.h>
#include <memory>

NRPEncoderCard::NRPEncoderCard(SATSolver *sat_solver, VarHandler *var_handler) : NRPEncoder(sat_solver, var_handler)
{
    std::cout << "c [NRPEncoderCard] Encoder initialized for " << number_of_nurses << " nurses over " << schedule_period << " days.\n";
}

NRPEncoderCard::~NRPEncoderCard()
{
    
}

void NRPEncoderCard::update_var_handler(const Clauses& clauses)
{
    for (Clause clause : clauses)
    {
        update_var_handler(clause);
    }
}

void NRPEncoderCard::update_var_handler(const Clause& clause)
{
    for (int lit : clause)
    {
        if (lit >= var_handler->peek_next_var())
        {
            var_handler->set_next_var(lit + 1);
        }
    }
}

void NRPEncoderCard::encode_at_least_20_work_shifts_every_28_days()
{
    std::vector<std::vector<int>> not_off_day(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            not_off_day[i][j] = -shift_schedule[i][j][3]; // Not off day
        }
    }

    PBConfig config = std::make_shared<PBConfigClass>();
    config->amk_encoder = AMK_ENCODER::CARD;
    PB2CNF* pb2cnf = new PB2CNF(config);

    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j <= schedule_period - 28; ++j)
        {
            std::vector<int> literals(not_off_day[i].begin() + j, not_off_day[i].begin() + j + 28);
            Clauses clauses;
            pb2cnf->encodeAtLeastK(literals, 20, clauses, var_handler->peek_next_var());
            for (Clause clause : clauses)
            {
                if (!clause.empty() && clause.back() == 0)
                {
                    clause.pop_back();      // Remove the lastest element if it is 0.
                }
                sat_solver->add_clause(clause);
            }
            update_var_handler(clauses);
        }
    }
}

void NRPEncoderCard::encode_at_least_4_off_days_every_14_days()
{
    std::vector<std::vector<int>> off_days(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            off_days[i][j] = shift_schedule[i][j][3]; // Off shift
        }
    }

    PBConfig config = std::make_shared<PBConfigClass>();
    config->amk_encoder = AMK_ENCODER::CARD;
    PB2CNF* pb2cnf = new PB2CNF(config);

    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j <= schedule_period - 14; ++j)
        {
            std::vector<int> literals(off_days[i].begin() + j, off_days[i].begin() + j + 14);
            Clauses clauses;
            pb2cnf->encodeAtLeastK(literals, 4, clauses, var_handler->peek_next_var());
            for (Clause clause : clauses)
            {
                if (!clause.empty() && clause.back() == 0)
                {
                    clause.pop_back();      // Remove the lastest element if it is 0.
                }
                sat_solver->add_clause(clause);
            }
            update_var_handler(clauses);
        }
    }
}

void NRPEncoderCard::encode_between_1_and_4_night_shifts_every_14_days()
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
    PBConfig config = std::make_shared<PBConfigClass>();
    config->amk_encoder = AMK_ENCODER::CARD;
    PB2CNF* pb2cnf = new PB2CNF(config);

    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j <= schedule_period - 14; ++j)
        {
            std::vector<int> literals(night_shift[i].begin() + j, night_shift[i].begin() + j + 14);
            Clauses clauses;
            pb2cnf->encodeAtMostK(literals, 4, clauses, var_handler->peek_next_var());
            for (Clause clause : clauses)
            {
                if (!clause.empty() && clause.back() == 0)
                {
                    clause.pop_back();      // Remove the lastest element if it is 0.
                }
                sat_solver->add_clause(clause);
            }
            update_var_handler(clauses);
        }
    }
}

void NRPEncoderCard::encode_between_4_and_8_evening_shifts_every_14_days()
{
    std::vector<std::vector<int>> evening_shifts(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            evening_shifts[i][j] = shift_schedule[i][j][1]; // Evening shifts
        }
    }

    PBConfig config = std::make_shared<PBConfigClass>();
    config->amk_encoder = AMK_ENCODER::CARD;
    PB2CNF* pb2cnf = new PB2CNF(config);

    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j <= schedule_period - 14; ++j)
        {
            std::vector<int> literals(evening_shifts[i].begin() + j, evening_shifts[i].begin() + j + 14);
            Clauses clauses;
            pb2cnf->encodeAtLeastK(literals, 4, clauses, var_handler->peek_next_var());
            for (Clause clause : clauses)
            {
                if (!clause.empty() && clause.back() == 0)
                {
                    clause.pop_back();      // Remove the lastest element if it is 0.
                }
                sat_solver->add_clause(clause);
            }
            update_var_handler(clauses);
        }
    }

    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j <= schedule_period - 14; ++j)
        {
            std::vector<int> literals(evening_shifts[i].begin() + j, evening_shifts[i].begin() + j + 14);
            Clauses clauses;
            pb2cnf->encodeAtMostK(literals, 8, clauses, var_handler->peek_next_var());
            for (Clause clause : clauses)
            {
                if (!clause.empty() && clause.back() == 0)
                {
                    clause.pop_back();      // Remove the lastest element if it is 0.
                }
                sat_solver->add_clause(clause);
            }
            update_var_handler(clauses);
        }
    }
}

void NRPEncoderCard::encode_night_shifts_cannot_appear_on_consecutive_days()
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

void NRPEncoderCard::encode_between_2_and_4_evening_or_night_shifts_every_7_days()
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

    PBConfig config = std::make_shared<PBConfigClass>();
    config->amk_encoder = AMK_ENCODER::CARD;
    PB2CNF* pb2cnf = new PB2CNF(config);

    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j <= schedule_period - 7; ++j)
        {
            std::vector<int> literals(evening_or_night_shift[i].begin() + j * 2, evening_or_night_shift[i].begin() + j * 2 + 7 * 2);
            Clauses clauses;
            pb2cnf->encodeAtLeastK(literals, 2, clauses, var_handler->peek_next_var());
            for (Clause clause : clauses)
            {
                if (!clause.empty() && clause.back() == 0)
                {
                    clause.pop_back();      // Remove the lastest element if it is 0.
                }
                sat_solver->add_clause(clause);
            }
            update_var_handler(clauses);
        }
    }

    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j <= schedule_period - 7; ++j)
        {
            std::vector<int> literals(evening_or_night_shift[i].begin() + j * 2, evening_or_night_shift[i].begin() + j * 2 + 7 * 2);
            Clauses clauses;
            pb2cnf->encodeAtMostK(literals, 4, clauses, var_handler->peek_next_var());
            for (Clause clause : clauses)
            {
                if (!clause.empty() && clause.back() == 0)
                {
                    clause.pop_back();      // Remove the lastest element if it is 0.
                }
                sat_solver->add_clause(clause);
            }
            update_var_handler(clauses);
        }
    }
}

void NRPEncoderCard::encode_at_most_6_work_shifts_every_7_days()
{
    std::vector<std::vector<int>> not_off_day(number_of_nurses, std::vector<int>(schedule_period, 0));
    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            not_off_day[i][j] = -shift_schedule[i][j][3]; // Not off day
        }
    }

    PBConfig config = std::make_shared<PBConfigClass>();
    config->amk_encoder = AMK_ENCODER::CARD;
    PB2CNF* pb2cnf = new PB2CNF(config);

    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j <= schedule_period - 7; ++j)
        {
            std::vector<int> literals(not_off_day[i].begin() + j, not_off_day[i].begin() + j + 7);
            Clauses clauses;
            pb2cnf->encodeAtMostK(literals, 6, clauses, var_handler->peek_next_var());
            for (Clause clause : clauses)
            {
                if (!clause.empty() && clause.back() == 0)
                {
                    clause.pop_back();      // Remove the lastest element if it is 0.
                }
                sat_solver->add_clause(clause);
            }
            update_var_handler(clauses);
        }
    }
}