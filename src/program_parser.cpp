#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <string>
#include <unordered_map>

#include "program.h"
#include "robot.h"

class ProgramParser {
    const std::unordered_map<std::string, Program::Operation> op_map = {
        {"NOP", Program::NOP},
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
        {Program::NOP, 0},
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
    struct ProgramError : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };
    
    Program ret;
    std::string program_str;
    std::istringstream f;
    std::string line;
    int code_cnt = 0;

    std::unordered_map<std::string, int> label_map;

public:
    ProgramParser(std::string program_str) : 
        program_str(program_str), f(program_str){}
private:

    std::string parse_label() {
        int l = 0, r = line.size();
        //remove comments
        for(int i = l;i < r;i++) {
            if(line[i] == '#')
                r = i;
        }
        while(l < r && isspace(line[l]))l++;
        while(l < r && isspace(line[r-1]))r--;
        if(l == r)return "";

        if(line[r-1] == ':') {
            if(r-l == 1)
                return "label cannot be empty";
            std::string label = line.substr(l, r-l-1);
            if(label.find(' ') != std::string::npos)
                return "label cannot have spaces: " + label;
            if(constant_map.find(label) != constant_map.end())
                return "label cannot have the same name as a constant: " + label;
            if(reg_map.find(label) != reg_map.end())
                return "label cannot have the same name as a register: " + label;
            if(label_map.find(label) != label_map.end())
                return "two labels cannot have the same name: " + label;

            label_map[label] = ret._labels.size();
            ret._labels.push_back(code_cnt);
        } else {
            code_cnt++;
        }
        return "";
    }

    std::string parse_instruction() {
        int l = 0, r = line.size();
        //remove comments
        for(int i = l;i < r;i++) {
            if(line[i] == '#')
                r = i;
        }
        while(l < r && isspace(line[l]))l++;
        while(l < r && isspace(line[r-1]))r--;
        if(l == r)return "";

        //skip labels
        if(line[r-1] == ':') 
            return "";

        Program::Instruction ins;

        for(int i = l;i <= r;i++) {
            if(isspace(line[i]) || i == r) {
                auto op = op_map.find(line.substr(l, i-l));
                if(op == op_map.end()) {
                    return "unkown op: " + line.substr(l, i-l);
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
                return "expected two operands";
            }
            for(int i = l;i <= r;i++) {
                if(isspace(line[i]) || i == r) {
                    auto reg = reg_map.find(line.substr(l, i-l));
                    if(reg == reg_map.end())
                        return "unkown register: " + line.substr(l, i-l);

                    ins.reg = reg->second;
                    l = i;
                    break;
                }
            }
        }
        while(l < r && isspace(line[l]))l++;
        if(oc >= 1) {
            if(l == r)
                return oc == 2 ? "expected two operands" : "expected one operand";

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
                                return "invalid operand: " + o;
                            }
                        } else {
                            auto c = constant_map.find(o);
                            if(c == constant_map.end())
                                return "invalid operand: " + o;
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
                        return "unknown label: " + line.substr(l, r-l);

                    ins.operand = label->second;
                    break;
                }
            }
        }

        ret._code.push_back(ins);
        return "";
    }
public:
    Program parse_program() {
        int line_cnt = -1;
        //parse the labels first to allow forward jumps
        while(std::getline(f, line)) {
            line_cnt++;
            std::string err = parse_label();
            if(err.size()) {
                ret._errors.push_back({
                        line_cnt,
                        err,
                        });
            }
        }
        //reset the string stream
        f.str(program_str);
        f.clear();
        line_cnt = -1;
        while(std::getline(f, line)) {
            line_cnt++;
            std::string err = parse_instruction();
            if(err.size()) {
                ret._errors.push_back({
                        line_cnt,
                        err,
                        });
            }
        }

        return ret;
    }
};

Program parse_program(std::string str) {
    //the code must be all caps
    std::transform(str.begin(), str.end(), str.begin(), toupper);
    ProgramParser program_parser(str);
    return program_parser.parse_program();
}

