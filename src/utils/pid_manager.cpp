#include "pid_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::vector<int> PIDManager::get_child_pids(int ppid)
{
    std::vector<int> childPIDs;
    std::ifstream file("/proc/" + std::to_string(ppid) + "/task/" + std::to_string(ppid) + "/children");

    if (!file.is_open())
    {
        std::cerr << "e [Pid] Unable to open /proc/" << ppid << "/task/" << ppid << "/children\n";
        return childPIDs;
    }

    std::string line;
    if (std::getline(file, line))
    {
        std::istringstream iss(line);
        int childPID;
        while (iss >> childPID)
        {
            childPIDs.push_back(childPID);
        }
    }
    file.close();
    return childPIDs;
};

std::vector<int> PIDManager::get_descendant_pids(int ppid)
{
    std::vector<int> descendantPIDs;

    std::vector<int> childPIDs = get_child_pids(ppid);

    for (int childPID : childPIDs)
    {
        descendantPIDs.push_back(childPID);

        std::vector<int> grandChildPIDs = get_descendant_pids(childPID);
        descendantPIDs.insert(descendantPIDs.end(), grandChildPIDs.begin(), grandChildPIDs.end());
    }

    return descendantPIDs;
};

size_t PIDManager::get_total_memory_usage(int pid)
{
    std::unordered_set<int> empty_except;
    return get_total_memory_usage(pid, empty_except);
};

size_t PIDManager::get_total_memory_usage(int pid, const std::unordered_set<int>& except_pids)
{
    size_t totalMemoryUsage = 0;

    std::vector<int> descendant_pids = get_descendant_pids(pid);

    if (except_pids.find(pid) == except_pids.end())
    {
        totalMemoryUsage += get_memory_usage(pid);
    }

    for (int descendant_pid : descendant_pids)
    {
        if (except_pids.find(descendant_pid) != except_pids.end())
            continue;

        totalMemoryUsage += get_memory_usage(descendant_pid);
    }

    // std::cout << "c [Lim] Process " << pid << " consumed total " << totalMemoryUsage / 1024.0 << " MB.\n";
    return totalMemoryUsage;
};

size_t PIDManager::get_memory_usage(int pid)
{
    std::ifstream file("/proc/" + std::to_string(pid) + "/status");
    if (!file.is_open())
    {
        std::cerr << "e [Pid] Unable to open /proc/" << pid << "/status\n";
        return 0;
    }

    std::string line;
    size_t memoryUsage = 0;

    while (std::getline(file, line))
    {
        if (line.find("VmRSS:") == 0)
        { // Look for the VmRSS field
            std::istringstream iss(line);
            std::string key, value, unit;
            iss >> key >> value >> unit;     // VmRSS: value unit
            memoryUsage = std::stoul(value); // Memory usage in kilobytes (KB)
            break;
        }
    }

    file.close();
    // std::cout << "c [Lim] Process " << pid << " consumed " << memoryUsage / 1024.0 << " MB.\n";
    return memoryUsage;
};