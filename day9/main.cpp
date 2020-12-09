#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>

const std::string INPUT_FILENAME = "input.txt";
const int RING_BUFFER_SIZE = 25;

// temporary storage because I don't want to drive everything off readInput.
// Numbers are definitely bigger than 32 bit.
std::vector<int64_t> Numbers;

struct Ring
{
    int64_t Buffer [RING_BUFFER_SIZE];
    int Position;

    void Push(int64_t Num)
    {
        Buffer[Position] = Num;
        Position = (Position+1) % RING_BUFFER_SIZE;
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

void printLowestAndHighestInRange(int Back, int Front)
{
    // This is interesting but I'm not sure if there's a better way.
    std::vector<int64_t>::const_iterator First = Numbers.begin() + Back;
    std::vector<int64_t>::const_iterator Last = Numbers.begin() + Front;
    std::vector<int64_t> Range(First, Last);

    std::sort(Range.begin(), Range.end());

    std::cout << "Encryption weakness sum is " << Range.front() << " + " << Range.back() << " = " << Range.front() + Range.back() << std::endl;
}

// Find the contiguous range of numbers in the original number list
// that add up to the invalid number. I think this can be done by
// having a moving range over the vector, moving the front of the
// range until the sum is greater than the number, and then moving
// the back of the range until the sum is less than the number.
void findInvalidNumberComponents(int64_t InvalidNum)
{
    int Front = 1; // has to be at least two numbers
    int Back = 0;

    // Loop until the front of the range goes over the end of the vector
    while (Front < Numbers.size())
    {
        // Sum the range of numbers
        int64_t Sum = 0;
        for (int i = Back; i <= Front; i++)
        {
            Sum += Numbers[i];
        }

        // If the Sum is lower than the InvalidNum, the front should be
        // moved one place forward.
        if (Sum < InvalidNum)
        {
            Front++;
            continue;
        }

        // If the Sum is higher than the InvalidNum, the back should be
        // moved one place forward. Although this seems intuitive if the
        // list of numbers is sorted in ascending order, it is not. Although,
        // the trend is increasing, so it feels right.
        if (Sum > InvalidNum)
        {
            Back++;
            continue;
        }

        // If the Sum is equal to the InvalidNum, we've got the right range.
        // Sort it, and output the lowest and highest numbers in the range.
        if (Sum == InvalidNum)
        {
            printLowestAndHighestInRange(Back, Front);
            return;
        }
    }
}

int main(int argc, char** argv)
{
    readInput();
    int64_t InvalidNum = findFirstInvalidNumber();
    std::cout << "First invalid number is " << InvalidNum << std::endl;

    findInvalidNumberComponents(InvalidNum);
}