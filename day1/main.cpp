#include <iostream>
#include <fstream>
#include <vector>
#include <set>

const int MAGIC_NUM = 2020;
const std::string INPUT_FILENAME = "input.txt";

// storage
std::vector<int> ValueVector;
std::set<int> ValueSet;

void readInput()
{
    std::ifstream ifs;
    ifs.open(INPUT_FILENAME);

    // current value to be read
    std::string value;

    while (!ifs.eof())
    {
        char c;
        ifs.get(c);

        // end of current value
        if (c == '\n' || ifs.eof())
        {
            int currentValue = std::stoi(value);
            value = ""; // reset line value

            ValueVector.push_back(currentValue);
            ValueSet.insert(currentValue);

            continue;
        }

        value.push_back(c);
    }

    ifs.close();
}

void part1Solution()
{
    for (int value : ValueVector)
    {
        int requiredNumber = MAGIC_NUM - value;

        // we have found both numbers
        if (ValueSet.count(requiredNumber) == 1)
        {
            int answer = value * requiredNumber;
            std::cout << "Part 1 answer is: " << answer << std::endl;
            return;
        }
    }

    std::cout << "Could not find a valid answer for part 1." << std::endl;
}

void part2Solution()
{
    // N^2, this is not great.
    for (int x : ValueVector)
    {
        for (int y : ValueVector)
        {
            int requiredNumber = MAGIC_NUM - x - y;

            if (ValueSet.count(requiredNumber) == 1)
            {
                int answer = x * y * requiredNumber;
                std::cout << "Part 2 answer is: " << answer << std::endl;
                return;
            }
        }
    }

    std::cout << "Could not find a valid answer for part 2." << std::endl;
}

int main(int argc, char** argv)
{
    readInput();
    part1Solution();
    part2Solution();
}