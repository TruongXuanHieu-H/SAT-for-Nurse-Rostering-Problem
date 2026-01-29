#ifndef SAT_SOLVER_H
#define SAT_SOLVER_H

#include <vector>
#include "var_handler.h"

typedef std::vector<int> Clause;
typedef std::vector<Clause> Clauses;

class SATSolver
{
public:
    virtual ~SATSolver() = default;

    virtual void add_clause(const Clause &c) = 0;
    virtual int solve() = 0;
    virtual std::vector<int> extract_result() = 0;

    int get_clause_count();

    void print_DIMACS_format();

protected:
    SATSolver(VarHandler *var_handler);

    VarHandler *var_handler;
    Clauses clauses;

};

#endif // SAT_SOLVER_H