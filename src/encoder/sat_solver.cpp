#include "sat_solver.h"

#include <iostream>

SATSolver::SATSolver(VarHandler *var_handler) : var_handler(var_handler)
{

}

int SATSolver::get_clause_count()
{
    return clauses.size();
}

void SATSolver::print_DIMACS_format()
{
    std::cout << "p cnf " << var_handler->count() << " " << get_clause_count() << "\n";
    for (const auto &clause : clauses)
    {
        for (const auto &lit : clause)
        {
            std::cout << lit << " ";
        }
        std::cout << "0\n";
    }
}