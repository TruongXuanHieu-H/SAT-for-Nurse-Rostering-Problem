#ifndef PID_MANAGER_H
#define PID_MANAGER_H

#include <vector>
#include <unordered_set>
#include <cstddef>

class PIDManager
{
public:
    static std::vector<int> get_child_pids(int ppid);       // Get child pids of the given pid
    static std::vector<int> get_descendant_pids(int pids);  // Get descendant pids of the given pid, include its child pids
    static size_t get_total_memory_usage(int pid);          // Get memory usage of the given pid and all of its descendant pids (kB)
    static size_t get_memory_usage(int pid);                // Get memory usage of the given pid (kB)
    static size_t get_total_memory_usage(int pid, 
        const std::unordered_set<int>& except_pids);      // Get memory usage of the given pid and all of its descendant pids, except except_pids (kB)
};

#endif // PID_MANAGER_H