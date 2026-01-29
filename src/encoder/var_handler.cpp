#include "var_handler.h"

#include <iostream>


VarHandler::VarHandler(int start_id) : start_id(start_id)
{
    next_var = start_id;

    std::cout << "c [VarHandler] Variable handler initialized. Starting variable ID: " << start_id << ".\n";
};

VarHandler::~VarHandler()
{
    std::cout << "c [VarHandler] Variable handler destroyed. Total variables used: " << count() << ".\n";  
};

int VarHandler::peek_next_var()
{
    return next_var;
};

int VarHandler::pop_next_var()
{
    return next_var++;
};

int VarHandler::count()
{
    return next_var - start_id;
};