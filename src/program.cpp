#include <algorithm>
#include <iostream>

#include "program.h"

void Program::tick(Robot& robot, Map& map) {
    if(robot._sleep_dur) {
        robot._sleep_dur--;
        return;
    }
    if(robot._pc >= _code.size())
        return;
    Instruction ins = _code[robot._pc];

    switch(ins.op) {
        case NOP:
            break;
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
        case CMP:
            get_register(robot, Robot::Cond) = 
                get_assignee(robot, ins)-get_operand(robot, ins);
            break;
        //Jumps have -1 to allow unconditional _pc++;
        case JMP:
            get_register(robot, Robot::PC) = 
                _labels[get_operand(robot, ins)] - 1;
            break;
        case JEQ:
            if(!get_register(robot, Robot::Cond)) {
                get_register(robot, Robot::PC) = 
                    _labels[get_operand(robot, ins)] - 1;
            }
            break;
        case JNE:
            if(get_register(robot, Robot::Cond)) {
                get_register(robot, Robot::PC) = 
                    _labels[get_operand(robot, ins)] - 1;
            }
            break;
        case LOOK:
            robot.look(map);
            break;
        case TURN:
            robot.turn((Direction)get_operand(robot, ins));
            break;
        case GO:
            robot.go_forward(map);
            break;
        case SLP:
            robot.sleep(get_operand(robot, ins));
            break;
        case USE:
            robot.use(map, get_operand(robot, ins));
            break;
        default:
            //TODO: add better error handling
            std::cout << "unkown command" << std::endl;
    }
    robot._pc++;
}

RegisterData& Program::get_register(Robot &robot, Robot::Register reg) {
    return robot.get_register(reg);
}

RegisterData& Program::get_assignee(Robot &robot, const Instruction ins) {
    return get_register(robot, ins.reg);
}

RegisterData Program::get_operand(Robot &robot, const Instruction ins) {
    if(ins.register_toggle) {
        return get_register(robot, (Robot::Register)ins.operand);
    } else {
        return ins.operand;
    }
}

bool Program::operator==(const Program& other) {
    return std::equal(_code.begin(), _code.end(), other._code.begin()) &&
           std::equal(_labels.begin(), _labels.end(), other._labels.begin());
}
