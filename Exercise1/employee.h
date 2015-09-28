//
//  employee.h
//  
//
//  Created by Rafael Kallis on 14/09/15.
//
//

#ifndef ____employee__
#define ____employee__

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <vector>
#include <algorithm>

struct employee{
    std::string surname;
    std::string name;
    uint32_t yearly_sal;
    uint8_t age;
    uint8_t clearance_l;
    
    bool operator<(struct employee const& _employee) const{return (yearly_sal < _employee.yearly_sal);}
};

typedef struct employee employee;

static inline struct employee new_employee(std::string const&,
                                             std::string const&,
                                             uint32_t const&,
                                             uint8_t const&,
                                             uint8_t const&);

void  fin_employee_vector(std::vector<struct employee>&,
                          std::string const&);
void  fout_employee_vector(std::vector<struct employee> const&,
                           std::string const&);

void  sort_employee_vector(std::vector<struct employee>&);

#endif /* defined(____employee__) */
