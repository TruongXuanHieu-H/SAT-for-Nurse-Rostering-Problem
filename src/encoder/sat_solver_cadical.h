#ifndef SAT_SOLVER_CADICAL_H
#define SAT_SOLVER_CADICAL_H

#include "sat_solver.h"
#include "../../solver/cadical/cadical.hpp"

class SATSolverCadical : public SATSolver
{
public:
    SATSolverCadical(VarHandler *var_handler);
    ~SATSolverCadical() override;

    void add_clause(const Clause &c) override;
    int solve() override;
    std::vector<std::vector<std::vector<bool>>> extract_result() override;

private:
    CaDiCaL::Solver *solver;
};

#endif // SAT_SOLVER_CADICAL_H