#include "sat_solver_cadical.h"
#include "../global_data.h"
#include <iostream>
#include <csignal>

SATSolverCadical::SATSolverCadical()
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
    std::cout << "c [SATSolverCaDiCaL] CaDiCaL solver destroyed.\n";
}

void SATSolverCadical::add_clause(const Clause &c)
{
    for (const auto &lit : c)
    {
        solver->add(lit);
    }
    solver->add(0);
}

int SATSolverCadical::solve()
{
    return solver->solve();
}

std::vector<int> SATSolverCadical::extract_result()
{
    std::vector<int> result;

    // Extract the assignment for all variables

    return result;
}
