#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <numeric>

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

void partOne()
{
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

int64_t CombinationsFromRange(int RangeStart, int RangeEnd)
{
    // Number of adaptors is one higher than the difference of
    // their positions.
    int AdaptorCount = RangeEnd - RangeStart + 1;

    // See the huge comment below for the calculations.
    switch(AdaptorCount)
    {
        case 2:
            return 1;
        case 3:
            return 2;
        case 4:
            return 4;
        default:
            return 4 + 3 * (AdaptorCount - 4);
    }
}

void partTwo()
{
    // Here we need to find all of the different combinations of adaptors that are
    // possible to get an unbroken chain from the outlet (0) to the device (highest
    // adaptor + 3) in various combination, WITH OR WITHOUT USING ALL ADAPTORS.
    //
    // Brute force is an undesirable option.
    //
    // We can skip the 3 Jolt gaps because these are non-negotiable - there's only
    // one combination of adaptors possible so only 1 variation. Similarly, either
    // by accident or design, we have no 2 Jolt gaps in the input data so we only
    // need to consider the 1 Jolt gaps.
    //
    // Maximum difference between adaptors can be 3 Jolts. We could have 1, 2, 3, 4
    // or more adaptors, but given the 3 Jolt limitation, there is a distinct number
    // of possible combinations only up to 4 adaptors. Then we just add another 3
    // possible connections (and thus combinations) for every additional adaptor with
    // a 1 Jolt difference.
    //
    // 2 adaptors = 1 combination (e.g. 0 -> 1)
    // 3 adaptors = 2 combinations (e.g. 0 -> 1 -> 2, 0 -> 2)
    // 4 adaptors = 4 combinations (0, 1, 2, 3) not going to bother mapping these out...
    // 5 adaptors = 7 combinations (0-4, 3 more than last time)
    //
    // So we only need to consider the distinct cases of 2 and 3, and for every
    // additional adaptor we add 3 more combinations.
    // Once we've figured this out for each unbroken chain of 1 Jolt differences,
    // we can multiply them all together to give the answer of how many combinations
    // there are amongst all adaptors (hopefully).
    // Complexity should be O(n).

    std::vector<int64_t> Combinations;

    int RangeStart = 0;
    int RangeEnd = 0;
    bool InRange = false;

    for (int i = 1; i < AdaptorJoltages.size(); i++)
    {
        // Current gap is 1, start or continue a range.
        if (AdaptorJoltages[i] - AdaptorJoltages[i-1] == 1)
        {
            // whether or not we are already in a range, update
            // the endpoint to be the current element
            RangeEnd = i;

            // start the range and loop around
            if (!InRange)
            {
                InRange = true;
                RangeStart = i - 1;
            }

            // If this is the final element, push the range so
            // we don't lose it.
            if (i == AdaptorJoltages.size() - 1)
            {
                Combinations.push_back(CombinationsFromRange(RangeStart, RangeEnd));
            }

            // nothing left to do until the range is ended
            continue;
        }

        // Assume here we saw a diff of 3.
        // Either end the previous range if one was in flight, or
        // keep on going.
        if (InRange)
        {
            // close off the range and add the combinations
            InRange = false;
            Combinations.push_back(CombinationsFromRange(RangeStart, RangeEnd));
        }
    }

    // hey look functional programming!
    int64_t TotalCombinations = std::accumulate(
        Combinations.begin(),
        Combinations.end(),
        (int64_t)1, // INITIAL VALUE MUST BE THE RIGHT TYPE!!!!
        std::multiplies<int64_t>()
    );

    std::cout << "Total number of possible combinations of adaptors: " << TotalCombinations << std::endl;
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

    partOne();
    partTwo();
}