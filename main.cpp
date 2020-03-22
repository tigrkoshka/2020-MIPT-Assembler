#include <iostream>
#include <fstream>
#include <bitset>
#include <ios>
#include <unordered_map>
#include <vector>
#include <cmath>

using namespace std;

ifstream fin;
fstream binary;
ofstream origin;

struct buffer {
    unsigned ui;
    int i;
};

union {
    char *buff;
    buffer *rd_buff;
} bin_rw;

const unsigned MEMORY_SIZE = 1u << 20u;
const unsigned REGISTER_NUM = 1u << 4u;

enum syscall_codes {
    EXIT = 0,
    SCANINT = 100,
    SCANDOUBLE = 101,
    PRINTINT = 102,
    PRINTDOUBLE = 103,
    GETCHAR = 104,
    PUTCHAR = 105
};

enum command_codes {
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

enum types {
    RM = 0,
    RR = 1,
    RI = 2,
    J = 3
};

union {
    unsigned long long ull;
    double dbl;
} ull_and_dbl;

unordered_map<unsigned, unsigned> codes_to_types = {
        {HALT,    RI},
        {SYSCALL, RI},
        {ADD,     RR},
        {ADDI,    RI},
        {SUB,     RR},
        {SUBI,    RI},
        {MUL,     RR},
        {MULI,    RI},
        {DIV,     RR},
        {DIVI,    RI},
        {LC,      RI},
        {SHL,     RR},
        {SHLI,    RI},
        {SHR,     RR},
        {SHRI,    RI},
        {AND,     RR},
        {ANDI,    RI},
        {OR,      RR},
        {ORI,     RI},
        {XOR,     RR},
        {XORI,    RI},
        {NOT,     RI},
        {MOV,     RR},
        {ADDD,    RR},
        {SUBD,    RR},
        {MULD,    RR},
        {DIVD,    RR},
        {ITOD,    RR},
        {DTOI,    RR},
        {PUSH,    RI},
        {POP,     RI},
        {CALL,    RR},
        {CALLI,   J},
        {RET,     J},
        {CMP,     RR},
        {CMPI,    RI},
        {CMPD,    RR},
        {JMP,     J},
        {JNE,     J},
        {JEQ,     J},
        {JLE,     J},
        {JL,      J},
        {JGE,     J},
        {JG,      J},
        {LOAD,    RM},
        {STORE,   RM},
        {LOAD2,   RM},
        {STORE2,  RM},
        {LOADR,   RR},
        {STORER,  RR},
        {LOADR2,  RR},
        {STORER2, RR},
};

unordered_map<string, unsigned> commands_to_codes = {
        {"halt",    HALT},
        {"syscall", SYSCALL},
        {"add",     ADD},
        {"addi",    ADDI},
        {"sub",     SUB},
        {"subi",    SUBI},
        {"mul",     MUL},
        {"muli",    MULI},
        {"div",     DIV},
        {"divi",    DIVI},
        {"lc",      LC},
        {"shl",     SHL},
        {"shli",    SHLI},
        {"shr",     SHR},
        {"shri",    SHRI},
        {"and",     AND},
        {"andi",    ANDI},
        {"or",      OR},
        {"ori",     ORI},
        {"xor",     XOR},
        {"xori",    XORI},
        {"not",     NOT},
        {"mov",     MOV},
        {"addd",    ADDD},
        {"subd",    SUBD},
        {"muld",    MULD},
        {"divd",    DIVD},
        {"itod",    ITOD},
        {"dtoi",    DTOI},
        {"push",    PUSH},
        {"pop",     POP},
        {"call",    CALL},
        {"calli",   CALLI},
        {"ret",     RET},
        {"cmp",     CMP},
        {"cmpi",    CMPI},
        {"cmpd",    CMPD},
        {"jmp",     JMP},
        {"jne",     JNE},
        {"jeq",     JEQ},
        {"jle",     JLE},
        {"jl",      JL},
        {"jge",     JGE},
        {"jg",      JG},
        {"load",    LOAD},
        {"store",   STORE},
        {"load2",   LOAD2},
        {"store2",  STORE2},
        {"loadr",   LOADR},
        {"storer",  STORER},
        {"loadr2",  LOADR2},
        {"storer2", STORER2},
};

unordered_map<unsigned, string> codes_to_commands = {
        {HALT,    "halt"},
        {SYSCALL, "syscall"},
        {ADD,     "add"},
        {ADDI,    "addi"},
        {SUB,     "sub"},
        {SUBI,    "subi"},
        {MUL,     "mul"},
        {MULI,    "muli"},
        {DIV,     "div"},
        {DIVI,    "divi"},
        {LC,      "lc"},
        {SHL,     "shl"},
        {SHLI,    "shli"},
        {SHR,     "shr"},
        {SHRI,    "shri"},
        {AND,     "and"},
        {ANDI,    "andi"},
        {OR,      "or"},
        {ORI,     "ori"},
        {XOR,     "xor"},
        {XORI,    "xori"},
        {NOT,     "not"},
        {MOV,     "mov"},
        {ADDD,    "addd"},
        {SUBD,    "subd"},
        {MULD,    "muld"},
        {DIVD,    "divd"},
        {ITOD,    "itod"},
        {DTOI,    "dtoi"},
        {PUSH,    "push"},
        {POP,     "pop"},
        {CALL,    "call"},
        {CALLI,   "calli"},
        {RET,     "ret"},
        {CMP,     "cmp"},
        {CMPI,    "cmpi"},
        {CMPD,    "cmpd"},
        {JMP,     "jmp"},
        {JNE,     "jne"},
        {JEQ,     "jeq"},
        {JLE,     "jle"},
        {JL,      "jl"},
        {JGE,     "jge"},
        {JG,      "jg"},
        {LOAD,    "load"},
        {STORE,   "store"},
        {LOAD2,   "load2"},
        {STORE2,  "store2"},
        {LOADR,   "loadr"},
        {STORER,  "storer"},
        {LOADR2,  "loadr2"},
        {STORER2, "storer2"},
};

unordered_map<string, unsigned> marks;

vector<unsigned> MEMORY(MEMORY_SIZE);
vector<unsigned> REGISTERS(REGISTER_NUM);

/*
 * 0 bit: ==
 * 1 bit: !=
 * 2 bit: >
 * 3 bit: <
 * 4 bit: >=
 * 5 bit: <=
 */

unsigned flags = 0;

unsigned current_command_number = 0;

unsigned main_address;

unsigned reserved_words = 0;

//------------------------Decompilation helpers-------------------------------

int get_operand_from_bin(unsigned bin, unsigned max_power_of_two) {
    if (bin >= 1u << max_power_of_two) {
        return -(int) ((1u << (max_power_of_two + 1)) - bin);
    } else {
        return (int) bin;
    }
}

//---------------------------Decompilation------------------------------------

string get_command_from_bin(unsigned bin) {
    string command;

    unsigned command_code = bin >> 24u;

    command += codes_to_commands[command_code] + ' ';

    switch (codes_to_types[command_code]) {
        case RM: {
            command += 'r' + to_string((bin >> 20u) & 15u) + ' ';

            command += to_string((bin << 12u) >> 12u);

            break;
        }

        case RR: {
            command += 'r' + to_string((bin >> 20u) & 15u) + ' ';
            command += 'r' + to_string((bin >> 16u) & 15u) + ' ';

            command += to_string(get_operand_from_bin((bin << 16u) >> 16u, 15u));

            break;
        }

        case RI: {
            command += 'r' + to_string((bin >> 20u) & 15u) + ' ';

            command += to_string((int) ((bin << 12u) >> 12u));

            break;
        }

        case J: {
            command += to_string(get_operand_from_bin((bin << 12u) >> 12u, 19u));

            break;
        }

        default: {
            cout << "ERROR: Unknown command type";
        }
    }

    return command;
}

void decompile() {
    binary.seekg(16);

    binary.read(bin_rw.buff, 4);
    unsigned code_size = bin_rw.rd_buff->ui;

    binary.seekg(512);

    unsigned command_bin;

    for (unsigned i = 0; i < (code_size >> 2u); i++) {
        binary.read(bin_rw.buff, 4);
        command_bin = bin_rw.rd_buff->ui;
        origin << get_command_from_bin(command_bin) << '\n';
    }
}

//------------------------Compilation helpers---------------------------------

void print_command_bin(unsigned bin) {
    if (bin == UINT32_MAX) {
        return;
    }

    unsigned count = 31;

    for (unsigned i = 1u << 31u; i > 0; i >>= 1u) {
        cout << ((bin & i) >> count);
        if (count == 24 || count == 20 || count == 16) {
            cout << ' ';
        }
        count--;
    }

    cout << '\n';
}

bool check_mark(const string &arg) {
    return marks.find(arg) != marks.end();
}

bool check_register(const string &arg) {
    if (arg.length() < 2 || arg[0] != 'r') {
        cout << "COMPILATION ERROR (at " + to_string(current_command_number) +
                "): Invalid argument (register expected)\n";
        return false;
    }

    if (arg[1] > '9' || arg[1] < '0' ||
        (arg.length() == 3 &&
         (arg[2] > '9' || arg[2] < '0' ||
          (arg[1] - '0') * 10 + (arg[2] - '0') > 15)) ||
        arg.length() > 3) {
        cout << "COMPILATION ERROR (at " + to_string(current_command_number) +
                "): Invalid register number (from 0 to 15 is valid)\n";
        return false;
    }

    return true;
}

unsigned get_register(const string &arg) {
    if (arg.length() == 2) {
        return (unsigned) (arg[1] - '0');
    } else {
        return (unsigned) ((arg[1] - '0') * 10 + (arg[2] - '0'));
    }
}

bool check_address(const string &arg) {
    int address = 0;

    if (marks.find(arg) != marks.end()) {
        return true;
    }

    try {
        address = stoi(arg);
    } catch (exception &e) {
        cout << "COMPILATION ERROR (at " + to_string(current_command_number) +
                "): Invalid argument (address expected)\n";
        return false;
    }

    if (address < 0 || (unsigned) address >= 1u << 20u) {
        cout << "COMPILATION ERROR (at " + to_string(current_command_number) +
                "): Invalid address (from 0 to 2^20-1 is valid)\n";
        return false;
    }

    return true;
}

unsigned get_address(const string &arg) {
    if (marks.find(arg) != marks.end()) {
        return marks[arg];
    }
    return (unsigned) stoi(arg);
}

bool check_operand(const string &arg, unsigned max_pow_of_two) {
    int operand = 0;

    try {
        operand = stoi(arg);
    } catch (exception &e) {
        cout << "COMPILATION ERROR (at " + to_string(current_command_number) +
                "): Invalid argument (operand expected)\n";
        return false;
    }

    if (operand < -((int) (1u << max_pow_of_two)) || operand >= (int) (1u << max_pow_of_two)) {
        cout << "COMPILATION ERROR (at " + to_string(current_command_number) + "): Invalid operand\n";
        return false;
    }

    return true;
}

unsigned get_bin_from_operand(const string &arg) {
    return (unsigned) stoi(arg);
}

//----------------------------Compilation-------------------------------------

unsigned get_bin_from_command(string &command) {
    string arg;

    unsigned command_code = commands_to_codes[command];

    unsigned command_bin = command_code << 24u;

    switch (codes_to_types[command_code]) {
        case RM: {
            fin >> arg;

            if (!check_register(arg)) return UINT32_MAX;
            command_bin += get_register(arg) << 20u;

            fin >> arg;

            if (!check_address(arg)) return UINT32_MAX;
            command_bin += get_address(arg);

            break;
        }

        case RR: {
            fin >> arg;

            if (!check_register(arg)) return UINT32_MAX;
            command_bin += get_register(arg) << 20u;

            fin >> arg;

            if (!check_register(arg)) return UINT32_MAX;
            command_bin += get_register(arg) << 16u;

            fin >> arg;

            if (!check_operand(arg, 15u)) return UINT32_MAX;
            command_bin += (get_bin_from_operand(arg) << 16u) >> 16u;

            break;
        }

        case RI: {
            fin >> arg;

            if (!check_register(arg)) return UINT32_MAX;
            command_bin += get_register(arg) << 20u;

            fin >> arg;

            if (!check_address(arg)) return UINT32_MAX;
            command_bin += get_address(arg);

            break;
        }

        case J: {
            fin >> arg;

            unsigned bin_arg;

            if (check_mark(arg)) {
                bin_arg = marks[arg];
            } else {
                if (!check_operand(arg, 19u)) return UINT32_MAX;
                bin_arg = get_bin_from_operand(arg);
            }

            command_bin += (bin_arg << 20u) >> 20u;

            break;
        }

        default: {
            cout << "ERROR: Unknown command type";
        }
    }

    return command_bin;
}

bool get_marks() {
    string command;

    bool return_status = true;

    while (fin >> command) {
        if (commands_to_codes.find(command) == commands_to_codes.end()) {
            if (command == "end") {
                break;
            }

            if (command == "word") {
                continue;
            }

            if (command[0] == ';') {
                getline(fin, command);
                continue;
            }

            if (command[command.length() - 1] != ':') {
                cout << "COMPILATION ERROR (at " + to_string(current_command_number) + "): invalid mark";
                return_status = false;
                break;
            }

            command.erase(command.length() - 1);

            if (marks.find(command) == marks.end()) {
                marks[command] = current_command_number;
            } else {
                cout << "COMPILATION ERROR (at " + to_string(current_command_number) + "): this mark already exists";
                return_status = false;
                break;
            }

        } else {
            unsigned type = codes_to_types[commands_to_codes[command]];

            string arg;
            fin >> arg;

            if (type != J) {
                fin >> arg;
            }

            if (type == RR) {
                fin >> arg;
            }

            current_command_number++;
        }
    }

    fin.seekg(0);
    current_command_number = 0;

    return return_status;
}

void make_binary() {
    // header
    binary << "ThisIsFUPM2Exec" << '\0';

    // code size
    bin_rw.rd_buff->ui = (current_command_number << 2u);
    binary.write(bin_rw.buff, 4);

    // const size
    bin_rw.rd_buff->ui = 0;
    binary.write(bin_rw.buff, 4);

    // data size
    bin_rw.rd_buff->ui = (reserved_words << 2u);
    binary.write(bin_rw.buff, 4);

    // entering point address
    bin_rw.rd_buff->ui = main_address;
    binary.write(bin_rw.buff, 4);

    // stack pointer
    bin_rw.rd_buff->ui = MEMORY_SIZE - 1;
    binary.write(bin_rw.buff, 4);

    // empty
    char *empty = (char *) malloc(476);
    binary.write(empty, 476);
    free(empty);

    // code
    for (unsigned i = 0; i < current_command_number; i++) {
        bin_rw.rd_buff->ui = MEMORY[i];
        binary.write(bin_rw.buff, 4);
    }
}

void compile() {
    bool check = get_marks();

    if (!check) {
        return;
    }

    string command;

    while (fin >> command) {
        if (command == "end") {
            fin >> command;

            if (marks.find(command) != marks.end()) {
                main_address = marks[command];
            } else {
                if (!check_operand(command, 19u)) {
                    cout << "COMPILATION ERROR (at " + to_string(current_command_number) + "): invalid main address";
                } else {
                    main_address = get_bin_from_operand(command);
                }
            }

            break;
        }

        if (command == "word") {
            reserved_words++;
            continue;
        }

        if (command[0] == ';') {
            getline(fin, command);
            continue;
        }

        if (commands_to_codes.find(command) == commands_to_codes.end()) {
            continue;
        }

        unsigned bin = get_bin_from_command(command);
        if (bin == UINT32_MAX) {
            MEMORY.clear();
            return;
        }

        MEMORY[current_command_number] = bin;

        current_command_number++;
    }

    make_binary();
    MEMORY.clear();
}

//------------------------ Execution helpers----------------------------------

double to_dbl(unsigned long long ull) {
    ull_and_dbl.ull = ull;
    double dbl = ull_and_dbl.dbl;
    ull_and_dbl.ull = 0;
    return dbl;
}

unsigned long long to_ull(double dbl) {
    ull_and_dbl.dbl = dbl;
    unsigned long long ull = ull_and_dbl.ull;
    ull_and_dbl.ull = 0;
    return ull;
}

unsigned long long get_two_reg(unsigned reg) {
    return (((unsigned long long) REGISTERS[reg + 1]) << 32u) + REGISTERS[reg];
}

void put_two_reg(unsigned long long to_put, unsigned reg) {
    REGISTERS[reg] = (to_put << 32u) >> 32u;
    REGISTERS[reg + 1] = to_put >> 32u;
}

template<typename T>
void write_cmp_to_flags(T a, T b) {
    unsigned to_write;

    if (a > b) {
        to_write =
                (0u << 0u) +
                (1u << 1u) +
                (1u << 2u) +
                (0u << 3u) +
                (1u << 4u) +
                (0u << 5u);
    } else {
        if (a == b) {
            to_write =
                    (1u << 0u) +
                    (0u << 1u) +
                    (0u << 2u) +
                    (0u << 3u) +
                    (1u << 4u) +
                    (1u << 5u);
        } else {
            to_write =
                    (0u << 0u) +
                    (1u << 1u) +
                    (0u << 2u) +
                    (1u << 3u) +
                    (0u << 4u) +
                    (1u << 5u);
        }
    }

    flags = ((flags >> 5u) << 5u) + to_write;
}

unsigned call(unsigned address_to) {
    unsigned to_return = REGISTERS[15];
    REGISTERS[14]--;
    MEMORY[REGISTERS[14]] = REGISTERS[15];
    REGISTERS[15] = address_to;
    return to_return;
}

void jump(unsigned condition, unsigned address_to) {
    if ((flags & (1u << condition)) != 0) {
        REGISTERS[15] = address_to;
    }
}

//-----------------------------Execution--------------------------------------

bool execute_bin_command(unsigned bin) {
    unsigned command_code = bin >> 24u;

    REGISTERS[15]++;

    switch (codes_to_types[command_code]) {
        case RM: {
            unsigned reg = (bin >> 20u) & 15u;

            unsigned mem = (bin << 12u) >> 12u;

            switch (command_code) {
                case LOAD: {
                    REGISTERS[reg] = MEMORY[mem];
                    break;
                }

                case STORE: {
                    MEMORY[mem] = REGISTERS[reg];
                    break;
                }

                case LOAD2: {
                    REGISTERS[reg] = MEMORY[mem];
                    REGISTERS[reg + 1] = MEMORY[mem];
                    break;
                }

                case STORE2: {
                    MEMORY[mem] = REGISTERS[reg];
                    MEMORY[mem + 1] = REGISTERS[reg + 1];
                    break;
                }

                default: {
                    break;
                }
            }

            break;
        }

        case RR: {
            unsigned reg1 = (bin >> 20u) & 15u;

            unsigned reg2 = (bin >> 16u) & 15u;

            int oper = get_operand_from_bin((bin << 16u) >> 16u, 15u);

            REGISTERS[reg2] += oper;

            switch (command_code) {
                case ADD: {
                    REGISTERS[reg1] += REGISTERS[reg2];
                    break;
                }

                case SUB: {
                    REGISTERS[reg1] -= REGISTERS[reg2];
                    break;
                }

                case MUL: {
                    unsigned long long res = ((unsigned long long) REGISTERS[reg1]) * (REGISTERS[reg2]);
                    REGISTERS[reg1] = (res << 32u) >> 32u;
                    REGISTERS[reg1 + 1] = res >> 32u;
                    break;
                }

                case DIV: {
                    unsigned long long first = get_two_reg(reg1);
                    if (REGISTERS[reg2] == 0) {
                        cout << "EXECUTION ERROR: division by zero";
                        return false;
                    }
                    REGISTERS[reg1] = first / (REGISTERS[reg2]);
                    REGISTERS[reg1 + 1] = first % (REGISTERS[reg2]);
                    break;
                }

                case SHL: {
                    REGISTERS[reg1] <<= REGISTERS[reg2];
                    break;
                }

                case SHR: {
                    REGISTERS[reg1] >>= REGISTERS[reg2];
                    break;
                }

                case AND: {
                    REGISTERS[reg1] &= REGISTERS[reg2];
                    break;
                }

                case OR: {
                    REGISTERS[reg1] |= REGISTERS[reg2];
                    break;
                }

                case XOR: {
                    REGISTERS[reg1] ^= REGISTERS[reg2];
                    break;
                }

                case MOV: {
                    REGISTERS[reg1] = REGISTERS[reg2];
                    break;
                }

                case ADDD: {
                    double first = to_dbl(get_two_reg(reg1));
                    double second = to_dbl(get_two_reg(reg2));

                    unsigned long long res = to_ull(first + second);

                    put_two_reg(res, reg1);

                    break;
                }

                case SUBD: {
                    double first = to_dbl(get_two_reg(reg1));
                    double second = to_dbl(get_two_reg(reg2));

                    unsigned long long res = to_ull(first - second);

                    put_two_reg(res, reg1);

                    break;
                }

                case MULD: {
                    double first = to_dbl(get_two_reg(reg1));
                    double second = to_dbl(get_two_reg(reg2));

                    unsigned long long res = to_ull(first * second);

                    put_two_reg(res, reg1);

                    break;
                }

                case DIVD: {
                    double first = to_dbl(get_two_reg(reg1));
                    double second = to_dbl(get_two_reg(reg2));

                    if (second == 0) {
                        cout << "EXECUTION ERROR: division by zero";
                        return false;
                    }

                    unsigned long long res = to_ull(first / second);

                    put_two_reg(res, reg1);

                    break;
                }

                case ITOD: {
                    put_two_reg(to_ull((double) (REGISTERS[reg2])), reg1);
                    break;
                }

                case DTOI: {
                    unsigned long long res = floor(to_dbl(get_two_reg(reg2)));

                    if (res >= (1ull << 32u)) {
                        cout << "EXECUTION ERROR: cannot convert double to int";
                        break;
                    }

                    REGISTERS[reg1] = res;

                    break;
                }

                case CALL: {
                    REGISTERS[reg1] = call(REGISTERS[reg2]);
                    break;
                }

                case CMP: {
                    write_cmp_to_flags<unsigned long long>(REGISTERS[reg1], REGISTERS[reg2]);
                    break;
                }

                case CMPD: {
                    double first = to_dbl(get_two_reg(reg1));
                    double second = to_dbl(get_two_reg(reg2));

                    write_cmp_to_flags<double>(first, second);

                    break;
                }

                case LOADR: {
                    REGISTERS[reg1] = MEMORY[REGISTERS[reg2]];
                    break;
                }

                case STORER: {
                    MEMORY[REGISTERS[reg2]] = REGISTERS[reg1];
                    break;
                }

                case LOADR2: {
                    REGISTERS[reg1] = MEMORY[REGISTERS[reg2]];
                    REGISTERS[reg1 + 1] = MEMORY[REGISTERS[reg2] + 1];

                    break;
                }

                case STORER2: {
                    MEMORY[REGISTERS[reg2]] = REGISTERS[reg1];
                    MEMORY[REGISTERS[reg2] + 1] = REGISTERS[reg1 + 1];

                    break;
                }

                default: {
                    break;
                }
            }

            REGISTERS[reg2] -= oper;

            break;
        }

        case RI: {
            unsigned reg = (bin >> 20u) & 15u;

            unsigned oper = get_operand_from_bin((bin << 12u) >> 12u, 19u);

            switch (command_code) {
                case HALT: {
                    REGISTERS[reg] = oper;
                    return false;
                }

                case SYSCALL: {
                    switch (oper) {
                        case EXIT: {
                            return false;
                        }

                        case SCANINT: {
                            cin >> REGISTERS[reg];
                            break;
                        }

                        case SCANDOUBLE: {
                            cin >> ull_and_dbl.dbl;
                            put_two_reg(ull_and_dbl.ull, reg);
                            ull_and_dbl.ull = 0;
                            break;
                        }

                        case PRINTINT: {
                            cout << get_operand_from_bin(REGISTERS[reg], 31);
                            break;
                        }

                        case PRINTDOUBLE: {
                            cout << to_dbl(get_two_reg(reg));
                            break;
                        }

                        case GETCHAR: {
                            char c;
                            cin >> c;
                            REGISTERS[reg] = (unsigned) c;
                            break;
                        }

                        case PUTCHAR: {
                            if (REGISTERS[reg] > 255) {
                                cout << "EXECUTION ERROR: cannot convert register value to a char";
                            } else {
                                cout << (char) REGISTERS[reg];
                            }
                            break;
                        }

                        default: {
                            cout << "EXECUTION ERROR: not a valid syscall code";
                        }
                    }
                    break;
                }

                case ADDI: {
                    REGISTERS[reg] += oper;
                    break;
                }

                case SUBI: {
                    REGISTERS[reg] -= oper;
                    break;
                }

                case MULI: {
                    unsigned long long res = ((unsigned long long) REGISTERS[reg]) * oper;
                    REGISTERS[reg] = (res << 32u) >> 32u;
                    REGISTERS[reg + 1] = res >> 32u;
                    break;
                }

                case DIVI: {
                    unsigned long long first = get_two_reg(reg);
                    if (oper == 0) {
                        cout << "EXECUTION ERROR: division by zero";
                        return false;
                    }
                    REGISTERS[reg] = first / oper;
                    REGISTERS[reg + 1] = first % oper;
                    break;
                }

                case LC: {
                    REGISTERS[reg] = oper;
                    break;
                }

                case SHLI: {
                    REGISTERS[reg] <<= oper;
                    break;
                }

                case SHRI: {
                    REGISTERS[reg] >>= oper;
                    break;
                }

                case ANDI: {
                    REGISTERS[reg] &= oper;
                    break;
                }

                case ORI: {
                    REGISTERS[reg] |= oper;
                    break;
                }

                case XORI: {
                    REGISTERS[reg] ^= oper;
                    break;
                }

                case NOT: {
                    REGISTERS[reg] = ~REGISTERS[reg];
                    break;
                }

                case PUSH: {
                    REGISTERS[14]--;
                    MEMORY[REGISTERS[14]] = REGISTERS[reg] + oper;
                    break;
                }

                case POP: {
                    REGISTERS[reg] = MEMORY[REGISTERS[14]] + oper;
                    REGISTERS[14]++;
                    break;
                }

                case CMPI: {
                    write_cmp_to_flags<unsigned long long>(REGISTERS[reg], oper);
                    break;
                }

                default: {
                    break;
                }
            }

            break;
        }

        case J: {
            unsigned oper = (bin << 12u) >> 12u;

            switch (command_code) {
                case CALLI: {
                    call(oper);
                    break;
                }

                case RET: {
                    REGISTERS[15] = MEMORY[REGISTERS[14]];
                    REGISTERS[14] += oper + 1;
                    break;
                }

                case JMP: {
                    REGISTERS[15] = oper;
                    break;
                }

                case JEQ: {
                    jump(0, oper);
                    break;
                }

                case JNE: {
                    jump(1, oper);
                    break;
                }

                case JG: {
                    jump(2, oper);
                    break;
                }

                case JL: {
                    jump(3, oper);
                    break;
                }

                case JGE: {
                    jump(4, oper);
                    break;
                }

                case JLE: {
                    jump(5, oper);
                    break;
                }

                default: {
                    break;
                }
            }

            break;
        }

        default: {
            cout << "ERROR: Unknown command type";
        }
    }

    return true;
}

void execute() {
    binary.seekg(16);
    binary.read(bin_rw.buff, 4);
    unsigned command_num = (bin_rw.rd_buff->ui >> 2u);

    binary.seekg(28);

    binary.read(bin_rw.buff, 4);
    REGISTERS[15] = bin_rw.rd_buff->ui;

    binary.read(bin_rw.buff, 4);
    REGISTERS[14] = bin_rw.rd_buff->ui;

    binary.seekg(512);

    for (unsigned i = 0; i < command_num; i++) {
        binary.read(bin_rw.buff, 4);
        MEMORY[i] = bin_rw.rd_buff->ui;
    }

    while (REGISTERS[15] < command_num) {
        if (!execute_bin_command(MEMORY[REGISTERS[15]])) {
            break;
        }
//        cout << MEMORY[MEMORY_SIZE - 1] << "                " << MEMORY_SIZE - REGISTERS[14] - 1 << '\n'
//             << MEMORY[MEMORY_SIZE - 2] << "                " << REGISTERS[15] <<'\n' << MEMORY[MEMORY_SIZE - 3]
//             << '\n' << MEMORY[MEMORY_SIZE - 4] << "\n\n";
    }
}

//-------------------------------Main-----------------------------------------

int main() {
    fin.open("input.fasm");
    binary.open("binary", fstream::in | fstream::out | fstream::binary | fstream::trunc);
    origin.open("origin.txt");
    bin_rw.buff = new char[4];

    compile();

    execute();

    decompile();

    fin.close();
    binary.close();
    origin.close();
}