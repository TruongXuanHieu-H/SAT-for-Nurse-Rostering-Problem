#include "sat_solver.h"

#include <iostream>

SATSolver::SATSolver(VarHandler *var_handler) : var_handler(var_handler)
{

}

void SATSolver::add_clause(const Clause& c)
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
}

int SATSolver::get_clause_count()
{
    return clauses.size();
}

int SATSolver::get_duplicate_clause_count()
{
    return duplicate_claus_count;
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