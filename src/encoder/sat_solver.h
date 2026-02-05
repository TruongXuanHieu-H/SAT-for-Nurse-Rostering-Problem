#ifndef SAT_SOLVER_H
#define SAT_SOLVER_H

#include <vector>
#include <unordered_set>
#include <algorithm>
#include "var_handler.h"

typedef std::vector<int> Clause;
typedef std::vector<Clause> Clauses;

struct ClauseHash {
    size_t operator()(const Clause &c) const noexcept {
        size_t h = c.size();
        for (int x : c) {
            h ^= static_cast<size_t>(x) + 0x9e3779b97f4a7c15ULL
                 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

typedef std::unordered_set<Clause, ClauseHash> ClauseStore;

class SATSolver
{
public:
    virtual ~SATSolver() = default;

    virtual void add_clause(const Clause &c);
    virtual int solve() = 0;
    virtual std::vector<std::vector<std::vector<bool>>> extract_result() = 0;

    int get_clause_count();
    int get_duplicate_clause_count();

    void print_DIMACS_format();

protected:
    SATSolver(VarHandler *var_handler);

    VarHandler *var_handler;
    Clauses clauses;
    ClauseStore clause_store;
    int duplicate_claus_count = 0;

};

#endif // SAT_SOLVER_H