#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <regex>
#include <set>

const std::string INPUT_FILENAME = "input.txt";

enum class Op
{
    Acc,
    Jmp,
    Nop
};

// program storage
std::vector<std::pair<Op, int>> Program;

void parseLine(const std::string& Line)
{
    std::regex InstructionRE("^(acc|jmp|nop) ([+-])([0-9]+)$");
    std::cmatch match;

    std::regex_match(Line.c_str(), match, InstructionRE);

    // Not sure if this helps or hinders
    Op OpCode;
    if (match[1] == "acc") OpCode = Op::Acc;
    if (match[1] == "jmp") OpCode = Op::Jmp;
    if (match[1] == "nop") OpCode = Op::Nop;

    int Operand = std::stoi(match[3]);
    if (match[2] == "-") Operand = -Operand;

    Program.push_back({OpCode, Operand});
}

void readInput()
{
    std::ifstream ifs;
    ifs.open(INPUT_FILENAME);

    if (ifs.fail())
    {
        std::cout << std::filesystem::current_path() << std::endl;
        throw("Failed to open input file");
    }

    while (!ifs.eof())
    {
        std::string Line;
        std::getline(ifs, Line);
        parseLine(Line);
    }
}

void executeProgram()
{
    int PC = 0;
    int Accumulator = 0;
    std::set<int> SeenOpCodes;

    while(true)
    {
        // check to see if we've executed this OpCode (position) before
        if (SeenOpCodes.count(PC) == 1) break;
        SeenOpCodes.emplace(PC);

        // Retrieve the current instruction from "memory"
        auto [OpCode, Operand] = Program.at(PC);

        // Logic for each operation
        if (OpCode == Op::Nop)
        {
            // this really could just be a default case at the bottom.
            PC++;
            continue;
        }

        if (OpCode == Op::Acc)
        {
            PC++;
            Accumulator += Operand;
            continue;
        }

        if (OpCode == Op::Jmp)
        {
            PC += Operand;
            continue;
        }
    }

    std::cout << "Value of the accumulator at point of infinite loop was " << Accumulator << std::endl;
}

int main(int argc, char** argv)
{
    readInput();
    executeProgram();
}