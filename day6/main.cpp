#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <set>

const std::string INPUT_FILENAME = "input.txt";

// Storage of answers per group. Each group's answers
// are represented as a set as we only care about distinct
// answers (so far).
std::vector<std::set<char>> Groups;

// parse answers from a set of lines representing a group
void parseGroup(const std::vector<std::string>& Lines)
{
    std::set<char> Answers;

    for (auto Line : Lines)
    {
        // parse each character individually and add to the set
        for (auto Char : Line)
        {
            Answers.insert(Char);
        }
    }

    Groups.push_back(std::move(Answers));
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

    // Buffer of lines of input while we gather all lines for a given group.
    std::vector<std::string> Buffer;

    while (!ifs.eof())
    {
        std::string Line;
        std::getline(ifs, Line);

        // Empty line signifies the end of a group - flush the buffer.
        if (Line == "")
        {
            parseGroup(Buffer);
            Buffer.clear();
            continue;
        }

        Buffer.push_back(Line);
    }

    // flush any remaining lines
    if (Buffer.size() > 0) parseGroup(Buffer);
}

void tallyGroupTotals()
{
    int totalsByGroup = 0;
    for (auto GroupAnswers : Groups)
    {
        totalsByGroup += GroupAnswers.size();
    }

    std::cout << "Total number of answers from all groups: " << totalsByGroup << std::endl;
}

int main(int argc, char** argv)
{
    readInput();
    tallyGroupTotals();
}