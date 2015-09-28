//
//  employee.cpp
//  
//
//  Created by Rafael Kallis on 14/09/15.
//
//

#include "employee.h"

static inline struct employee new_employee(std::string const& surname,
                                           std::string const& name,
                                           uint32_t const& yearly_sal,
                                           uint8_t const& age,
                                           uint8_t const& clearance_l)
{
    struct employee _new_employee;
    
    _new_employee.surname        = surname;
    _new_employee.name           = name;
    _new_employee.yearly_sal     = yearly_sal;
    _new_employee.age            = age;
    _new_employee.clearance_l    = clearance_l;
    
    return _new_employee;
}

void fin_employee_vector(std::vector<struct employee>& employee_vector,
                         std::string const& file_name)
{
    std::string buf;
    
    std::string tmp_surname;
    std::string tmp_name;
    uint32_t    tmp_yearly_sal;
    uint8_t     tmp_age;
    uint8_t     tmp_clearance_l;
    
    std::ifstream in(file_name);
    if (in.is_open()){
        while (std::getline(in,buf))
        {
            std::stringstream ss(buf);
            ss >> tmp_surname;
            ss >> tmp_name;
            ss >> tmp_yearly_sal;
            ss >> tmp_age;
            ss >> tmp_clearance_l;
            employee_vector.push_back(new_employee(tmp_surname,
                                                    tmp_name,
                                                    tmp_yearly_sal,
                                                    tmp_age,
                                                    tmp_clearance_l));
        }
        in.close();
    }else{
        std::cout << file_name << " not found\nTerminating\n";
        exit(EXIT_FAILURE);
    }
}

void  fout_employee_vector(std::vector<struct employee> const& employee_vector,
                           std::string const& file_name)
{
    std::ofstream out(file_name);
    if (out.is_open()) {
        for (int i=0; i < employee_vector.size(); i++)
        {
            out     << employee_vector.at(i).surname        << " "
                    << employee_vector.at(i).name           << " "
                    << employee_vector.at(i).yearly_sal     << " "
                    << employee_vector.at(i).age            << " "
                    << employee_vector.at(i).clearance_l    << "\n";
        }
        out.close();
    } else {
        std::cout << file_name << " not found\nTerminating\n";
        exit(EXIT_FAILURE);
    }
}

void sort_employee_vector(std::vector<struct employee>& employee_vector)
{
    std::sort(employee_vector.begin(), employee_vector.end());
}
