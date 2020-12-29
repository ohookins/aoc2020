#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <regex>
#include <vector>
#include <set>

const std::string INPUT_FILENAME = "input.txt";
const std::regex MemRE("^mem\\[(\\d+)\\] = (\\d+)$");
const std::regex MaskRE("^mask = ([01X]+)$");

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

void ParseMaskPart1(const std::string& MaskString, uint64_t& OnesMask, uint64_t& ZeroesMask, std::vector<int>& FloatingBits)
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

            case 'X':
                FloatingBits.push_back(End-i);
                break;
        }
    }
}

std::set<std::string> MaskPrefixes;

void ParseMaskPart2(const std::string& MaskString, uint64_t& OnesMask, uint64_t& ZeroesMask, std::vector<int>& FloatingBits)
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

            case 'X':
                FloatingBits.push_back(End-i);
                break;
        }
    }

    std::string MaskPrefix = MaskString.substr(0, 16);
    if (MaskPrefixes.count(MaskPrefix) == 1)
    {
        std::cout << "Have already seen mask with prefix: " << MaskPrefix << std::endl;
    }
    else
    {
        MaskPrefixes.insert(MaskPrefix);
    }
}

void ParseMemPart1(const uint64_t& Address, uint64_t Value, const uint64_t& OnesMask, const uint64_t& ZeroesMask, std::map<uint64_t, uint64_t>& Program)
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

void ParseMemPart2(uint64_t Address, const uint64_t& Value, const uint64_t& OnesMask, std::vector<int> FloatingBits, std::map<uint64_t, uint64_t>& Program)
{
    return;
}

uint64_t SumValues(const std::map<uint64_t, uint64_t>& Program)
{
    uint64_t Sum = 0;

    for (const auto& [Address, Value] : Program)
    {
        Sum += Value;
    }

    return Sum;
}

void Part1(const std::vector<std::string>& ProgramListing)
{
    // Sparse storage of each number in the program.
    // Masks are interpreted as we go.
    std::map<uint64_t, uint64_t> Program;

    // Going to try to do this with different masks for ones and zeroes,
    // so that we can do the masking in just two steps rather than iteratively.
    uint64_t OnesMask = 0;
    uint64_t ZeroesMask = 0;
 
    std::vector<int> FloatingBits; // not used in this part

    std::smatch Match;

    for (auto Line : ProgramListing)
    {
        // Detect and parse the masks
        if (std::regex_match(Line, Match, MaskRE))
        {
            ParseMaskPart1(Match[1].str(), OnesMask, ZeroesMask, FloatingBits);
            continue;
        }

        // Detect and add the number to the program
        if (std::regex_match(Line, Match, MemRE))
        {
            int Address = std::stoull(Match[1].str());
            uint64_t Value = std::stoull(Match[2].str());
            ParseMemPart1(Address, Value, OnesMask, ZeroesMask, Program);
        }
    }

    std::cout << "Part 1, sum of all values is " << SumValues(Program) << std::endl;
}

void Part2(const std::vector<std::string>& ProgramListing)
{
    // Sparse storage of each number in the program.
    // Masks are interpreted as we go.
    std::map<uint64_t, uint64_t> Program;

    // Part 2 effectively doesn't use the zeroes mask, but it uses the
    // Xs as floating bits which we can just set memory addresses from
    // later.
    uint64_t OnesMask = 0;
    uint64_t ZeroesMask = 0;
    std::vector<int> FloatingBits;
    std::smatch Match;

    for (auto Line : ProgramListing)
    {
        // Detect and parse the masks
        if (std::regex_match(Line, Match, MaskRE))
        {
            ParseMaskPart2(Match[1].str(), OnesMask, ZeroesMask, FloatingBits);
            continue;
        }

        // Detect and add the number to the program
        if (std::regex_match(Line, Match, MemRE))
        {
            int Address = std::stoull(Match[1].str());
            uint64_t Value = std::stoull(Match[2].str());
            ParseMemPart2(Address, Value, OnesMask, FloatingBits, Program);
        }
    }

    std::cout << "Part 2, sum of all values is " << SumValues(Program) << std::endl;
    for (auto p : MaskPrefixes)
    {
        [p](){
            for (auto q : MaskPrefixes)
            {
                bool matches = true;
                for (int i = 0; i <= 15; i++)
                {
                    if ((p[i] == '1' || p[i] == '0') && (q[i] == '1' || q[i] == '0') && (p[i] != q[i]))
                    {
                        matches = false;
                    }
                }

                if (matches && p != q)
                {
                    std::cout << p << " overlaps with " << q << std::endl;
                }
            }
        }();
    }
}

int main(int argc, char** argv)
{
    std::vector<std::string> ProgramListing;

    // this would actually be ideal for a fiber or coroutine
    ReadInput([&ProgramListing](std::string Line) {
        ProgramListing.push_back(Line);
    });

    Part1(ProgramListing);
    Part2(ProgramListing);
}