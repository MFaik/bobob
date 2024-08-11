#pragma once

#include <string>
#include <vector>
#include <utility>

struct Program;

Program parse_program(std::string str);
std::vector<std::pair<std::string, std::string>> get_op_strs();
std::vector<std::string> get_reg_strs();
std::vector<std::string> get_constant_strs();

