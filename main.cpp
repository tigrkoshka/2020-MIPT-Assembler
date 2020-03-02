#include <iostream>
#include <map>
#include <vector>

using namespace std;

const unsigned MEMORY_SIZE = 1u << 20u;
const unsigned REGISTER_NUM = 1u << 4u;

enum code {
    HALT = 0,
    SYSCALL = 1,
    ADD = 2,
    ADDI = 3,
    SUB = 4,
    SUBI = 5,
    MUL = 6,
    MULI = 7,
    DIV = 8,
    DIVI = 9,
    LC = 12,
    SHL = 13,
    SHLI = 14,
    SHR = 15,
    SHRI = 16,
    AND = 17,
    ANDI = 18,
    OR = 19,
    ORI = 20,
    XOR = 21,
    XORI = 22,
    NOT = 23,
    MOV = 24,
    ADDD = 32,
    SUBD = 33,
    MULD = 34,
    DIVD = 35,
    ITOD = 36,
    DTOI = 37,
    PUSH = 38,
    POP = 39,
    CALL = 40,
    CALLI = 41,
    RET = 42,
    CMP = 43,
    CMPI = 44,
    CMPD = 45,
    JMP = 46,
    JNE = 47,
    JEQ = 48,
    JLE = 49,
    JL = 50,
    JGE = 51,
    JG = 52,
    LOAD = 64,
    STORE = 65,
    LOAD2 = 66,
    STORE2 = 67,
    LOADR = 68,
    LOADR2 = 69,
    STORER = 70,
    STORER2 = 71
};

enum command_types {
    RM = 0,
    RR = 1,
    RI = 2,
    J = 3
};

map<string, pair<unsigned, unsigned>> commands = {
        {"halt",    {HALT,    RI}},
        {"syscall", {SYSCALL, RI}},
        {"add",     {ADD,     RR}},
        {"addi",    {ADDI,    RI}},
        {"sub",     {SUB,     RR}},
        {"subi",    {SUBI,    RI}},
        {"mul",     {MUL,     RR}},
        {"muli",    {MULI,    RI}},
        {"div",     {DIV,     RR}},
        {"divi",    {DIVI,    RI}},
        {"lc",      {LC,      RI}},
        {"shl",     {SHL,     RR}},
        {"shli",    {SHLI,    RI}},
        {"shr",     {SHR,     RR}},
        {"shri",    {SHRI,    RI}},
        {"and",     {AND,     RR}},
        {"andi",    {ANDI,    RI}},
        {"or",      {OR,      RR}},
        {"ori",     {ORI,     RI}},
        {"xor",     {XOR,     RR}},
        {"xori",    {XORI,    RI}},
        {"not",     {NOT,     RI}},
        {"mov",     {MOV,     RR}},
        {"addd",    {ADDD,    RR}},
        {"subd",    {SUBD,    RR}},
        {"muld",    {MULD,    RR}},
        {"divd",    {DIVD,    RR}},
        {"itod",    {ITOD,    RR}},
        {"dtoi",    {DTOI,    RR}},
        {"push",    {PUSH,    RI}},
        {"pop",     {POP,     RI}},
        {"call",    {CALL,    RR}},
        {"calli",   {CALLI,   RI}},
        {"ret",     {RET,     RI}},
        {"cmpl",    {CMP,     RR}},
        {"cmpi",    {CMPI,    RI}},
        {"cmpd",    {CMPD,    RR}},
        {"jmp",     {JMP,     J}},
        {"jne",     {JNE,     J}},
        {"jeq",     {JEQ,     J}},
        {"jle",     {JLE,     J}},
        {"jl",      {JL,      J}},
        {"jge",     {JGE,     J}},
        {"jg",      {JG,      J}},
        {"load",    {LOAD,    RM}},
        {"store",   {STORE,   RM}},
        {"load2",   {LOAD2,   RM}},
        {"store2",  {STORE2,  RM}},
        {"loadr",   {LOADR,   RR}},
        {"storer",  {STORER,  RR}},
        {"loadr2",  {LOADR2,  RR}},
        {"storer2", {STORER2, RR}},
};

vector<unsigned> MEMORY(MEMORY_SIZE);
vector<unsigned> REGISTERS(REGISTER_NUM);
unsigned flags;

bool check_register(const string &arg) {
    if (arg.length() < 2 || arg[0] != 'r') {
        cout << "COMPILATION ERROR: Invalid argument (register expected)";
        return false;
    }

    if (arg[1] > '9' || arg[1] < '0' ||
        (arg.length() == 3 &&
         (arg[2] > '9' || arg[2] < '0' ||
          (arg[1] - '0') * 10 + (arg[2] - '0') > 15)) ||
        arg.length() > 3) {
        cout << "COMPILATION ERROR: Invalid register number (from 0 to 15 is valid)";
        return false;
    }

    return true;
}

unsigned get_register(const string &arg) {
    if (arg.length() == 2) {
        return (unsigned) (arg[1] - '0') << 20u;
    } else {
        return (unsigned) ((arg[1] - '0') * 10 + (arg[2] - '0')) << 20u;
    }
}

bool check_address(const string &arg) {
    int address = 0;

    try {
        address = stoi(arg);
    } catch (exception &e) {
        cout << "COMPILATION ERROR: Invalid argument (address expected)";
        return false;
    }

    if (address < 0 || (unsigned) address >= 1u << 20u) {
        cout << "COMPILATION ERROR: Invalid address";
        return false;
    }

    return true;
}

unsigned get_address(const string &arg) {
    return (unsigned) stoi(arg);
}

bool check_operand(const string &arg, unsigned max_pow_of_two) {
    int operand = 0;

    try {
        operand = stoi(arg);
    } catch (exception &e) {
        cout << "COMPILATION ERROR: Invalid argument (operand expected)";
        return false;
    }

    if (operand < -((int) (1u << max_pow_of_two)) || operand >= (int) (1u << max_pow_of_two)) {
        cout << "COMPILATION ERROR: Invalid operand";
        return false;
    }

    return true;
}

unsigned get_operand(const string &arg) {
    return (unsigned) stoi(arg);
}

unsigned get_command_code(const string &command) {
    string arg;

    unsigned command_code = commands[command].first << 24u;

    switch (commands[command].second) {
        case RM: {
            cin >> arg;

            if (!check_register(arg)) return 0;
            command_code += get_register(arg) << 20u;

            cin >> arg;

            if (!check_address(arg)) return 0;
            command_code += get_address(arg);

            break;
        }

        case RR: {
            cin >> arg;

            if (!check_register(arg)) return 0;
            command_code += get_register(arg) << 20u;

            cin >> arg;

            if (!check_register(arg)) return 0;
            command_code += get_register(arg) << 16u;

            cin >> arg;

            if (!check_operand(arg, 15u)) return 0;
            command_code += get_operand(arg);

            cout << RR;

            break;
        }

        case RI: {
            cin >> arg;

            if (!check_register(arg)) return 0;
            command_code += get_register(arg) << 20u;

            cin >> arg;

            if (!check_address(arg)) return 0;
            command_code += get_address(arg);

            break;
        }

        case J: {
            cin >> arg;

            if (!check_operand(arg, 19u)) return 0;
            command_code += get_operand(arg);

            break;
        }

        default: {
            cout << "ERROR: Unknown command type";
        }
    }

    return command_code;
}

int main() {
    string command;
    cin >> command;
    if (commands.find(command) == commands.end()) {
        cout << "Unknown command";
        return 0;
    }

    unsigned code = get_command_code(command);

    unsigned count = 31;

    for (unsigned i = 1u << 31u; i > 0; i >>= 1u) {
        cout << ((code & i) >> count);
        if (count == 24 || count == 20 || count == 16) {
            cout << ' ';
        }
        count--;
    }
}