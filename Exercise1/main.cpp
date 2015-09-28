//
//  main.cpp
//  
//
//  Created by Rafael Kallis on 14/09/15.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "employee.h"

int main(int argc, char* argv[]){
    
    if (argc != 2)
    {
        std::cout << "No source file defined / undefined input\nTerminating\n";
        return EXIT_FAILURE;
    }
    
    std::vector<employee> employee_vector;
    
    fin_employee_vector(employee_vector, argv[1]);
    
    sort_employee_vector(employee_vector);
    
    fout_employee_vector(employee_vector, "sorted_db.txt");
    
    return EXIT_SUCCESS;
}