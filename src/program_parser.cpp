#include <algorithm>
#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "program.h"
#include "robot.h"

const std::unordered_map<std::string, Program::Operation> op_map = {
    {"MOV", Program::MOV},
    {"ADD", Program::ADD},
    {"SUB", Program::SUB},
    {"MUL", Program::MUL},
    {"DIV", Program::DIV},
    {"MOD", Program::MOD},
    {"NOT", Program::NOT},
    {"AND", Program::AND},
    {"OR", Program::OR},
    {"XOR", Program::XOR},
    {"CMP", Program::CMP},
    {"JMP", Program::JMP},
    {"JEQ", Program::JEQ},
    {"JNE", Program::JNE},
    {"LOOK", Program::LOOK},
    {"TURN", Program::TURN},
    {"GO", Program::GO},
    {"SLP", Program::SLP},
    {"USE", Program::USE},
    {"SEL", Program::SEL},
};

const std::unordered_map<std::string, Robot::Register> reg_map = {
    {"A", Robot::A},
    {"B", Robot::B},
    {"PC", Robot::PC},
    {"COND", Robot::Cond},
    {"INPUT", Robot::Input},
    {"SEL", Robot::Sel},
};

//TODO: add EVERY SINGLE ITEM here
const std::unordered_map<std::string, int> constant_map = {
    {"NORTH", 0},
    {"EAST", 1},
    {"SOUTH", 2},
    {"WEST", 3},
    {"UP", 4},
    {"RIGHT", 5},
    {"DOWN", 6},
    {"LEFT", 7},
};

const std::unordered_map<Program::Operation, int> operand_cnt = {
    {Program::MOV, 2},
    {Program::ADD, 2},
    {Program::SUB, 2},
    {Program::MUL, 2},
    {Program::DIV, 2},
    {Program::MOD, 2},
    {Program::NOT, 1},
    {Program::AND, 2},
    {Program::OR, 2},
    {Program::XOR, 2},
    {Program::CMP, 2},
    {Program::JMP, 0},
    {Program::JEQ, 0},
    {Program::JNE, 0},
    {Program::LOOK, 0},
    {Program::TURN, 1},
    {Program::GO, 0},
    {Program::SLP, 1},
    {Program::USE, 0},
    {Program::SEL, 1},
};

//TODO: add error handling
Program parse_program(std::string str) {
    //the code must be all caps
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);

    Program ret;
    std::istringstream f(str);
    std::string line;

    std::unordered_map<std::string, int> label_map;
    
    int line_cnt = -1;
    int code_cnt = 0;
    //parse the labels first 
    //to allow forward jumps
    while(std::getline(f, line)) {
        line_cnt++;
        try{
            int l = 0, r = line.size();
            //remove comments
            for(int i = l;i < r;i++) {
                if(line[i] == '#')
                    r = i;
            }
            while(l < r && isspace(line[l]))l++;
            while(l < r && isspace(line[r-1]))r--;
            if(l == r)continue;

            if(line[r-1] == ':') {
                if(r-l == 1)
                    throw std::runtime_error("label cannot be empty");
                std::string label = line.substr(l, r-l);
                if(label.find(' ') != std::string::npos)
                    throw std::runtime_error("label cannot have spaces: " + label);
                if(constant_map.find(label) != constant_map.end())
                    throw std::runtime_error("label cannot have the same name as a constant: " + label);
                if(reg_map.find(label) != reg_map.end())
                    throw std::runtime_error("label cannot have the same name as a register: " + label);
                if(label_map.find(label) != label_map.end())
                    throw std::runtime_error("two labels cannot have the same name: " + label);

                label_map[line.substr(l, r-l-1)] = ret._labels.size();
                ret._labels.push_back(code_cnt);
            } else {
                code_cnt++;
            }
        } catch(const std::runtime_error &err) {
            ret._errors.push_back({
                line_cnt,
                err.what(),
            });
        }
    }
    //reset the string stream
    f.str(str);
    f.clear();
    line_cnt = -1;
    while(std::getline(f, line)) {
        line_cnt++;
        try {
            int l = 0, r = line.size();
            //remove comments
            for(int i = l;i < r;i++) {
                if(line[i] == '#')
                    r = i;
            }
            while(l < r && isspace(line[l]))l++;
            while(l < r && isspace(line[r-1]))r--;
            if(l == r)continue;

            //skip labels
            if(line[r-1] == ':') {
                continue;
            }

            Program::Instruction ins;
            *((int*)((void*)&ins)) = -1;

            for(int i = l;i <= r;i++) {
                if(isspace(line[i]) || i == r) {
                    auto op = op_map.find(line.substr(l, i-l));
                    if(op == op_map.end()) {
                        throw std::runtime_error("unkown op: " + line.substr(l, i-l));
                    }
                    ins.op = op->second;
                    l = i;
                    break;
                }
            }
            while(l < r && isspace(line[l]))l++;
            int oc = operand_cnt.at(ins.op);
            if(oc == 2) {
                if(l == r) {
                    throw std::runtime_error("expected two operands");
                }
                for(int i = l;i <= r;i++) {
                    if(isspace(line[i]) || i == r) {
                        auto reg = reg_map.find(line.substr(l, i-l));
                        if(reg == reg_map.end())
                            throw std::runtime_error("unkown register: " + line.substr(l, i-l));

                        ins.reg = reg->second;
                        l = i;
                        break;
                    }
                }
            }
            while(l < r && isspace(line[l]))l++;
            if(oc >= 1) {
                if(l == r)
                    throw std::runtime_error(oc == 2 ? "expected two operands" : "expected one operand");
                
                bool isnum = true;
                for(int i = l;i <= r;i++) {
                    if(isspace(line[i]) || i == r) {
                        std::string o = line.substr(l, i-l);
                        auto reg = reg_map.find(o);
                        if(reg != reg_map.end()) {
                            ins.register_toggle = true;
                            ins.operand = reg->second;
                        } else {
                            ins.register_toggle = false;
                            if(isnum) {
                                try {
                                    ins.operand = stoi(o);
                                } catch(...) {
                                    throw std::runtime_error("invalid operand: " + o);
                                }
                            } else {
                                auto c = constant_map.find(o);
                                if(c == constant_map.end())
                                    throw std::runtime_error("invalid operand: " + o);
                                ins.operand = c->second;
                            }
                        }
                        break;
                    } else {
                        isnum &= isdigit(line[i]);
                    }
                }
            }

            if(ins.op == Program::JMP || ins.op == Program::JNE 
                    || ins.op == Program::JEQ) {
                for(int i = l;i <= r;i++) {
                    if(isspace(i) || i == r) {
                        auto label = label_map.find(line.substr(l, r-l));
                        if(label == label_map.end()) 
                            throw std::runtime_error("unknown label: " + line.substr(l, r-l));

                        ins.operand = label->second;
                        break;
                    }
                }
            }

            ret._code.push_back(ins);
        } catch(const std::runtime_error &err) {
            ret._errors.push_back({
                line_cnt,
                err.what(),
            });
        }
    }

    return ret;
}
