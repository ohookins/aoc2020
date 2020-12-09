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

// also store indices of locations of Nop and Jmp commands
// so that we can find and swap them, and brute-force test
// the resulting programs
std::vector<int> NopIndex;
std::vector<int> JmpIndex;

void parseLine(const std::string& Line)
{
    std::regex InstructionRE("^(acc|jmp|nop) ([+-])([0-9]+)$");
    std::cmatch match;

    std::regex_match(Line.c_str(), match, InstructionRE);

    // Not sure if this helps or hinders
    Op OpCode;
    if (match[1] == "acc") OpCode = Op::Acc;
    if (match[1] == "jmp")
    {
        OpCode = Op::Jmp;

        // store the location in the index
        JmpIndex.push_back(Program.size());
    }
    if (match[1] == "nop")
    {
        OpCode = Op::Nop;

        // store the location in the index
        NopIndex.push_back(Program.size());
    }

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

bool executeProgram(int &OutAccumulator)
{
    int PC = 0;
    std::set<int> SeenOpCodes;

    while(true)
    {
        // check to see if we've executed this OpCode (position) before
        if (SeenOpCodes.count(PC) == 1) return false;
        SeenOpCodes.emplace(PC);

        // Check to see if we have an invalid jump instruction
        // which makes the program counter beyond the end of the program.
        // In a better emulator we would have some kind of CPU exception here.
        // NOTE: It seems that jumping arbitrarily far off the end of the program
        // is considered a valid exit. I was expecting for only nop/acc instructions
        // falling off the end by a single instruction to count as graceful exit,
        // but this is not the case.
        if (PC >= Program.size()) return true;

        // Retrieve the current instruction from "memory"
        auto [OpCode, Operand] = Program.at(PC);

        // Jump is the only Opcode that explicitly
        // changes the PC to a different value instead
        // of incrementing it, so ensure we go back to the start
        // of the loop here.
        if (OpCode == Op::Jmp)
        {
            PC += Operand;
            continue;
        }

        if (OpCode == Op::Acc)
        {
            // we increment PC outside the if
            OutAccumulator += Operand;
        }

        // Nop ends up here as well as Acc, both of which
        // continue to the next instruction.
        PC++;
    }

    return true;
}

// brute-force our way through the nop and jmp indices to find the
// corrupted instruction.
int fixCorruption()
{
    int Accumulator = 0;

    for (int PC : NopIndex)
    {
        // swap the nop for a jmp
        auto [OpCode, Operand] = Program.at(PC);
        Program[PC] = {Op::Jmp, Operand};

        // test the program and see if it terminates successfully
        bool terminatedSuccessfully = executeProgram(Accumulator);
        if (terminatedSuccessfully) return Accumulator;

        // swap the nop back for the next iteration
        Program[PC] = {OpCode, Operand};

        Accumulator = 0;
    }

    for (int PC : JmpIndex)
    {
        // swap the jmp for a nop
        auto [OpCode, Operand] = Program.at(PC);
        Program[PC] = {Op::Nop, Operand};

        // test the program and see if it terminates successfully
        bool terminatedSuccessfully = executeProgram(Accumulator);
        if (terminatedSuccessfully) return Accumulator;

        // swap the jmp back for the next iteration
        Program[PC] = {OpCode, Operand};

        Accumulator = 0;
    }

    return -1; // this is not a good signal.
}

int main(int argc, char** argv)
{
    readInput();

    int Accumulator = 0;
    executeProgram(Accumulator);
    std::cout << "Value of the accumulator at point of infinite loop was " << Accumulator << std::endl;

    Accumulator = fixCorruption();
    std::cout << "Accumulator after fixing bug was " << Accumulator << std::endl;
}