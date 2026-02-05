#include "sat_solver_cadical.h"
#include "../global_data.h"
#include <iostream>
#include <csignal>

SATSolverCadical::SATSolverCadical(VarHandler *var_handler) : SATSolver(var_handler)
{
    solver = new CaDiCaL::Solver();
    std::cout << "c [SATSolverCaDiCaL] Initializing CaDiCaL (version " << solver->version() << ").\n";
    std::string sat_configuration = "sat";
    int res = solver->configure(sat_configuration.data());
    std::cout << "c [SATSolverCaDiCaL] Configuring CaDiCaL as --" << sat_configuration << " (" << res << ").\n";
}

SATSolverCadical::~SATSolverCadical()
{
    if (solver) 
    {
        delete solver;
        solver = nullptr;
    }
    std::cout << "c [SATSolverCaDiCaL] Total clauses used: " << get_clause_count() << "\n";
    std::cout << "c [SATSolverCaDiCal] Duplicate clauses found: " << get_duplicate_clause_count() << "\n";
}

void SATSolverCadical::add_clause(const Clause &c)
{
    Clause normalized = c;
    std::sort(normalized.begin(), normalized.end());

    auto [it, inserted] = clause_store.insert(normalized);
    if (!inserted)
    {
        duplicate_claus_count++;
        return;
    }

    clauses.push_back(normalized);

    for (const auto &lit : normalized)
    {
        solver->add(lit);
    }
    solver->add(0);
}

int SATSolverCadical::solve()
{
    return solver->solve();
}

std::vector<std::vector<std::vector<bool>>> SATSolverCadical::extract_result()
{
    int number_of_nurses = GlobalData::get_number_nurses();
    int schedule_period = GlobalData::get_schedule_period();

    std::vector<std::vector<std::vector<bool>>> result(number_of_nurses, std::vector<std::vector<bool>>(schedule_period, std::vector<bool>(4, false)));

    for (int i = 0; i < number_of_nurses; ++i)
    {
        for (int j = 0; j < schedule_period; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                int var = k + j * 4 + i * schedule_period * 4 + 1; // Variable numbering starts from 1
                result[i][j][k] = (solver->val(var) > 0);
            }
        }
    }
    return result;
}
