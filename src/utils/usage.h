#ifndef USAGE_H
#define USAGE_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

class Helper
{
private:
    static const std::vector<std::pair<std::string, std::string>> option_list;

public:
    static void print_usage();
};

#endif // USAGE_H