#pragma once
#include <vector>
#include <string>

#include "robot.h"

class Program {
public:
    enum Operation {
        MOV,
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        NOT,
        AND,
        OR,
        XOR,
        CMP,
        JMP,
        JEQ,
        JNE,
        LOOK = 64,
        TURN,
        GO,
        SLP,
        USE,
        SEL,
    };
    //TODO: rename this "operand" and "reg" to something else
    struct Instruction {
        int operand : 16;
        bool register_toggle : 1;
        Robot::Register reg : 8;
        Operation op : 7;

        Instruction(){}
    };
    struct Error {
        int line;
        std::string txt;
    };
//TODO: removing this private is hacky
//private:
    std::vector<Instruction> _code;
    std::vector<unsigned int> _labels;
    std::vector<Error> _errors;
public:
    void tick(Robot &robot);

    int& get_register(Robot &robot, Robot::Register reg);
    int& get_assignee(Robot &robot, Instruction ins);
    int get_operand(Robot &robot, Instruction ins);
};
