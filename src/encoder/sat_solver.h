#ifndef SAT_SOLVER_H
#define SAT_SOLVER_H

#include <vector>

typedef std::vector<int> Clause;
typedef std::vector<Clause> Clauses;

class SATSolver
{
public:
    SATSolver() = default;
    virtual ~SATSolver() = default;

    virtual void add_clause(const Clause &c) = 0;
    virtual int solve() = 0;
    virtual std::vector<int> extract_result() = 0;

    int get_clause_count() const { return clauses.size(); }

protected:
    bool do_split = false;
    Clauses clauses;

};

#endif // SAT_SOLVER_H