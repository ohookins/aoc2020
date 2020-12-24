#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <regex>

const std::string INPUT_FILENAME = "input.txt";

void ReadInput(std::function<void(std::string)> function)
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
        function(Line);
    }
}

// For debugging only
void PrintBinaryString(const uint64_t& Value)
{
    // Have to start with MSB end due to print order.
    for (int i = 35; i >= 0; i--)
    {
        std::cout << ((Value >> i) & 1);
    }
    std::cout << std::endl;
}

void ParseMask(const std::string& MaskString, uint64_t& OnesMask, uint64_t& ZeroesMask)
{
    // Clear the masks to begin with, to simplify things.
    OnesMask = ZeroesMask = 0;

    // Start from the LSB end
    int End = MaskString.size() - 1;
    for (int i = End; i >= 0; i--)
    {
        switch (MaskString[i])
        {
            case '0':
                ZeroesMask |= (uint64_t)1 << (End-i);
                break;

            case '1':
                OnesMask |= (uint64_t)1 << (End-i);
                break;
        }
    }
}

void ParseMem(const int& Address, uint64_t Value, const uint64_t& OnesMask, const uint64_t& ZeroesMask, std::map<int, uint64_t>& Program)
{
    // Apply ones and zeroes masks to input value.
    // Ones mask is easy enough - do an OR.
    Value |= OnesMask;

    // Zeroes mask is to set anything that is currently an 1 to a 0 if the zeroes
    // mask has a 1.
    // (M) 1 + 1 (V) = 0
    // (M) 1 + 0 (V) = 0
    // (M) 0 + 1 (V) = 1
    // (M) 0 + 0 (V) = 0
    // Essentially this means Value AND NOT ZeroesMask.
    Value &= ~ZeroesMask;

    // Store in Program memory at Address.
    // map.insert will not overwrite, whereas operator[] will,
    // and we need this to happen.
    Program[Address] = Value;
}

void SumValues(const std::map<int, uint64_t>& Program)
{
    uint64_t Sum = 0;

    for (const auto& [Address, Value] : Program)
    {
        Sum += Value;
    }

    std::cout << "Part 1, sum of all values is " << Sum << std::endl;
}

int main(int argc, char** argv)
{
    // Sparse storage of each number in the program.
    // Masks are interpreted as we go.
    std::map<int, uint64_t> Program;

    // Going to try to do this with different masks for ones and zeroes,
    // so that we can do the masking in just two steps rather than iteratively.
    uint64_t OnesMask = 0;
    uint64_t ZeroesMask = 0;

    // this would actually be ideal for a fiber or coroutine
    ReadInput([&OnesMask, &ZeroesMask, &Program](std::string Line) {
        std::regex MemRE("^mem\\[(\\d+)\\] = (\\d+)$");
        std::regex MaskRE("^mask = ([01X]+)$");
        std::smatch Match;

        // Detect and parse the masks
        if (std::regex_match(Line, Match, MaskRE))
        {
            ParseMask(Match[1].str(), OnesMask, ZeroesMask);
            return;
        }

        // Detect and add the number to the program
        if (std::regex_match(Line, Match, MemRE))
        {
            int Address = std::stoi(Match[1].str());
            uint64_t Value = std::stoull(Match[2].str());
            ParseMem(Address, Value, OnesMask, ZeroesMask, Program);
        }
    });

    SumValues(Program);
}