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

void compareValues()
{
    for (int value : ValueVector)
    {
        int requiredNumber = MAGIC_NUM - value;

        // we have found both numbers
        if (ValueSet.count(requiredNumber) == 1)
        {
            int answer = value * requiredNumber;
            std::cout << "Answer is: " << answer << std::endl;
            return;
        }
    }

    std::cout << "Could not find a valid answer." << std::endl;
}

int main(int argc, char** argv)
{
    readInput();
    compareValues();
}