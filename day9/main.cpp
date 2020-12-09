#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

const std::string INPUT_FILENAME = "input.txt";

// temporary storage because I don't want to drive everything off readInput.
// Numbers are definitely bigger than 32 bit.
std::vector<int64_t> Numbers;

struct Ring
{
    int64_t Buffer [25];
    int Position;

    void Push(int64_t Num)
    {
        Buffer[Position] = Num;
        // this is stupid
        Position = (Position+1) % (sizeof(Buffer) / sizeof(int64_t));
    }
} NumberRing;

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
        Numbers.push_back(std::stoll(Line));
    }
}

// Check through the ring buffer for two numbers
// that add up to the supplied number. This could 
// be a lot more efficient and is currently N^2.
bool numberIsValid(int64_t Num)
{
    // Iterate over the ring buffer to find the first number
    for (int64_t FirstNum : NumberRing.Buffer)
    {
        int64_t RequiredNumber = Num - FirstNum;

        // Iterate again (could use a set here) to see if the
        // required number to make the sum exists in the buffer.
        for (int64_t SecondNum : NumberRing.Buffer)
        {
            if (SecondNum == RequiredNumber) return true;
        }
    }

    return false;
}

int64_t findFirstInvalidNumber()
{
    // Get an iterator for convenience in the vector of numbers
    auto it = Numbers.begin();

    // Push the first 25 numbers into the ring buffer
    for (int i = 0; i < 25; i++)
    {
        NumberRing.Push(*it);
        it++;
    }

    // Check each of the subsequent numbers for validation criteria
    while (it != Numbers.end())
    {
        int64_t CurrentNumber = *it;

        // Check the latest number to see whether it is invalid, and
        // if so, return it.
        if (!numberIsValid(CurrentNumber)) return CurrentNumber;

        // If it is valid, it takes the place at the end of the ring
        // buffer and pushes the oldest one out.
        NumberRing.Push(CurrentNumber);

        // Move the iterator to the next number for comparison.
        it++;
    }

    return 0;
}

int main(int argc, char** argv)
{
    readInput();
    int64_t InvalidNum = findFirstInvalidNumber();
    std::cout << "First invalid number is " << InvalidNum << std::endl;
}