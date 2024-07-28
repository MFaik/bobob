#include <iostream>

#include "code_runner.h"

CodeRunner g_code_runner;

void CodeRunner::setup(std::vector<ins_t> code, std::vector<ins_t> labels) {
    // _code = code;
    // _labels = labels;
    swap(code, _code);
    swap(labels, _labels);
}

void CodeRunner::tick(Robot &robot) {
    robot._pc %= _code.size();
    ins_t ins = _code[robot._pc];
    
    switch(ins>>op_shift) {
        case MOV:
            get_assignee(robot, ins) = get_operand(robot, ins);
            break;
        case ADD:
            get_assignee(robot, ins) += get_operand(robot, ins);
            break;
        case SUB:
            get_assignee(robot, ins) -= get_operand(robot, ins);
            break;
        case MUL:
            get_assignee(robot, ins) *= get_operand(robot, ins);
            break;
        case DIV:
            get_assignee(robot, ins) /= get_operand(robot, ins);
            break;
        case MOD:
            get_assignee(robot, ins) %= get_operand(robot, ins);
            break;
        case NOT:
            get_assignee(robot, ins) = ~get_operand(robot, ins);
            break;
        case AND:
            get_assignee(robot, ins) &= get_operand(robot, ins);
            break;
        case OR:
            get_assignee(robot, ins) |= get_operand(robot, ins);
            break;
        case XOR:
            get_assignee(robot, ins) ^= get_operand(robot, ins);
            break;
        case NOR:
            get_assignee(robot, ins) |= ~get_operand(robot, ins);
            break;
        case CMP:
            get_register(robot, Robot::Cond) = 
                get_assignee(robot, ins)-get_operand(robot, ins);
            break;
        //Jumps have -1 to allow unconditional _pc++;
        case JMP:
            get_register(robot, Robot::PC) = 
                _labels[get_operand(robot, ins)%_labels.size()] - 1;
            break;
        case JEQ:
            if(!get_register(robot, Robot::Cond)) {
                get_register(robot, Robot::PC) = 
                    _labels[get_operand(robot, ins)%_labels.size()] - 1;
            }
            break;
        case JNE:
            if(get_register(robot, Robot::Cond)) {
                get_register(robot, Robot::PC) = 
                    _labels[get_operand(robot, ins)%_labels.size()] - 1;
            }
            break;
        case LOOK:
            robot.look();
            break;
        case TURN:
            robot.turn((Direction)get_operand(robot, ins));
            break;
        case GO:
            robot.go_forward();
            break;
        case SLP:
            robot.sleep(get_operand(robot, ins));
            break;
        case USE:
            robot.use();
            break;
        case SEL:
            robot.select(get_operand(robot, ins));
            break;
        default:
            //TODO: add better error handling
            std::cout << "unkown command" << std::endl;
    }
    robot._pc++;
}

int& CodeRunner::get_register(Robot &robot, Robot::Register reg) {
    return robot.*Robot::regs[reg];
}

int& CodeRunner::get_assignee(Robot &robot, const ins_t ins) {
    auto reg = (Robot::Register)
        ((ins >> (constant_bit_len+1)) & ((1<<register_bit_len)-1));
    return get_register(robot, reg);
}

int CodeRunner::get_operand(Robot &robot, const ins_t ins) {
    if(ins & (1<<operand_toggle_bit)) {
        auto reg = (Robot::Register)(ins&((1<<register_bit_len)-1));
        return get_register(robot, reg);
    } else {
        return ins & ((1<<constant_bit_len)-1);
    }
}

