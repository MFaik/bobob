#pragma once
#include <vector>
#include <stdint.h>

#include "robot.h"

typedef uint32_t ins_t;

class CodeRunner {
private:
    //ooooooorrrrrrrrtcccccccccccccccc
    //12345671234567811234567890123456
    //o-> operation, r -> register aka assignee 
    //c-> constant/register aka operand, t-> register toggle

    static constexpr int op_shift = 25;
    static constexpr int operand_toggle_bit = 16;
    static constexpr int register_bit_len = 8;
    static constexpr int constant_bit_len = 16;

    enum Instruction {
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
        NOR,
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

    std::vector<ins_t> _code;
    std::vector<ins_t> _labels;
public:
    void setup(std::vector<ins_t> code, std::vector<ins_t> labels);

    void tick(Robot &robot);

    int& get_register(Robot &robot, Robot::Register reg);
    inline int& get_assignee(Robot &robot, const ins_t ins);
    inline int get_operand(Robot &robot, const ins_t ins);
};
