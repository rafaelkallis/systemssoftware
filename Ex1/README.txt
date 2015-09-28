Systems Software, Exercise 1
Rafael Kallis 14-708-887

The program starts by checking if a command line argument was passed.

A Vector of type Employee is being created.

It is passed to fin_employee_vector with the command line argument.
If the string passed doesn't exist, the program terminates, else it continues looping through each line.
With the help of stringstream, formatted data is being passed to tmp variables, which are then used to push a new employee in the vector.
The Program then sorts the vector. Note that there is an overloaded operator in the struct file in order to not use a functor or a function pointer.
In the end fout_employee_vector is being called. First it checks if "sorted_db.txt" exists. If so, it loops through the vector and passes the sorted data to the txt file.
In the end the destructor of vector is being called and all data is removed without any leaks (notice that there are no pointers used, all parameters are passed by reference).

