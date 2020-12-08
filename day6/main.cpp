#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <map>

const std::string INPUT_FILENAME = "input.txt";

// Storage of answers per group. Each group's answers
// are represented as a map as we now also need to know
// which questions have an answer from everyone in the
// group.
// - Groups
//   - Group
//     - Group size
//     - Map of answers to answer count
std::vector<std::pair<int, std::map<char, int>>> Groups;

// parse answers from a vector of lines representing a group
void parseGroup(const std::vector<std::string>& Lines)
{
    std::map<char, int> Answers;

    for (auto Line : Lines)
    {
        // parse each character individually and add to the set
        for (auto Answer : Line)
        {
            // Do the usual dance of if exists in the map etc
            if (Answers.count(Answer) == 1)
            {
                Answers[Answer] = Answers.at(Answer) + 1;
                continue;
            }

            Answers[Answer] = 1;
        }
    }

    // Each group is a pair of the group size and the map of answers to counts.
    // This allows us to know for which answers we got an answer from everyone
    // in the group.
    Groups.push_back({Lines.size(), std::move(Answers)});
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

// Count all distinct answers in each group
void tallyDistinctGroupTotals()
{
    int totalsByGroup = 0;

    for (auto GroupAnswers : Groups)
    {
        totalsByGroup += GroupAnswers.second.size();
    }

    std::cout << "Total number of answers from all groups: " << totalsByGroup << std::endl;
}

// Count all answers for which everyone in the group answered
void tallyAllAnsweredGroupTotals()
{
    int totalsWhereEveryoneAnswered = 0;

    for (auto GroupAnswers : Groups)
    {
        int groupSize = GroupAnswers.first;

        // Iterate through map by answer and count of answers in the group.
        // We add one to the tally if the count of answers is the same as
        // the group size == everyone in the group answered it.
        for (const auto& [Answer, Count] : GroupAnswers.second)
        {
            if (Count == groupSize) totalsWhereEveryoneAnswered++;
        }
    }

    std::cout << "Everyone in each group answered this number of questions: " << totalsWhereEveryoneAnswered << std::endl;
}

int main(int argc, char** argv)
{
    readInput();
    tallyDistinctGroupTotals();
    tallyAllAnsweredGroupTotals();
}