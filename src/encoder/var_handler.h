#ifndef VAR_HANDLER_H
#define VAR_HANDLER_H
class VarHandler
{
public:
    VarHandler(int start_id);
    ~VarHandler();

    int peek_next_var();        // Get the next variable ID without incrementing counter (see only)
    int pop_next_var();         // Get the next variable ID and increment counter
    int count();                // Get the total number of variables used

    void set_next_var(int next); // Set the next variable

private:
    int start_id;
    int next_var;
};
#endif