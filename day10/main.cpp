#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>

const std::string INPUT_FILENAME = "input.txt";

// This isn't great but TIL you can't use a capturing lambda
// as a function pointer, so I can't use the lambda method below
// with a local variable.
std::vector<int> AdaptorJoltages;

void ReadInput(void (*function)(std::string))
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

int main(int argc, char** argv)
{
    // Put the outlet joltage in the list so we can generate a difference
    // to that like all the other adaptors.
    AdaptorJoltages.push_back(0);

    // Read and sort adaptors by joltage
    ReadInput([](std::string Line) { AdaptorJoltages.push_back(std::stoi(Line)); });
    std::sort(AdaptorJoltages.begin(), AdaptorJoltages.end());

    // Highest device rating is the highest jolt adaptor + 3
    // Push this to the end so we have a final difference to calculate.
    int DeviceRating = AdaptorJoltages.back() + 3;
    AdaptorJoltages.push_back(DeviceRating);

    int OneJoltDiffs = 0, ThreeJoltDiffs = 0;

    for (int i = 1; i < AdaptorJoltages.size(); i++)
    {
        int Difference = AdaptorJoltages[i] - AdaptorJoltages[i-1];
        switch(Difference)
        {
            case 1:
                OneJoltDiffs++;
                break;
            case 3:
                ThreeJoltDiffs++;
                break;
        }
    }

    std::cout << "Part 1 answer is " << OneJoltDiffs * ThreeJoltDiffs << std::endl;
}