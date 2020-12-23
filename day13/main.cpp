#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <regex>
#include <cmath>
#include <numeric>
#include <algorithm>

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

std::vector<std::string> ParseTimetable(const std::string Timetable)
{
    std::vector<std::string> Buses;

    // this is probably a bit risky but I'll assume we can rely on the comma separation
    std::regex BusRE("^([x\\d]+),?");
    std::smatch Match;
    std::string SearchString(Timetable);
    while (std::regex_search(SearchString, Match, BusRE))
    {
        Buses.push_back(Match[1].str());
        SearchString = Match.suffix();
    }

    return Buses;
}

std::vector<int64_t> FilterRunningBuses(std::vector<std::string> AllBuses)
{
    std::vector<int64_t> RunningBuses;
    for (auto Bus : AllBuses)
    {
        if (Bus == "x") continue;

        RunningBuses.push_back(std::stoll(Bus));
    }
    return RunningBuses;
}

void DetermineEarliestBus(std::vector<std::string> AllBuses, int Timestamp)
{
    // Filter out the 'x's from the bus list
    // It would be nice to do a kind of .map / .filter thing here
    // with std::transform but I can't see a non-verbose way yet.
    std::vector<int64_t> RunningBuses = FilterRunningBuses(AllBuses);

    int64_t EarliestTimestamp = INT64_MAX;
    int64_t EarliestBus = 0;

    for (int64_t Bus : RunningBuses)
    {
        // should give either the exact or rounded-down answer?
        int64_t Quotient = Timestamp / Bus;
        
        // determine the earliest time close to the timestamp
        // that this bus leaves
        int64_t EarliestTime = 0;
        if (Quotient * Bus == Timestamp) EarliestTime = Timestamp;
        else EarliestTime = (Quotient + 1) * Bus;

        // compare to the existing lowest number and replace if lower
        if (EarliestTime < EarliestTimestamp)
        {
            EarliestTimestamp = EarliestTime;
            EarliestBus = Bus;
        }
    }

    int64_t Part1Answer = EarliestBus * (EarliestTimestamp - Timestamp);
    std::cout << "The earliest bus is " << EarliestBus << " at time " << EarliestTimestamp << std::endl;
    std::cout << "Would need to wait " << EarliestTimestamp - Timestamp << " minutes for it." << std::endl;
    std::cout << "Part1 Answer: " << Part1Answer << std::endl;
}

// Determine the Bezout's Identity (s & t) values for a given coprime pair of
// integers.
// Shamelessly stolen from some textbook somewhere, with some small modification of the
// return parameters.
std::pair<int64_t, int64_t> GetBezoutIdentity(int64_t a, int64_t b, int64_t s1 = 1, int64_t s2 = 0, int64_t t1 = 0, int64_t t2 = 1)
{
    int64_t quotient = floor(a / b);
    int64_t remainder = a - quotient * b; // can't use remquo as it doesn't support int64_t

	int64_t s3 = s1 - quotient * s2;
	int64_t t3 = t1 - quotient * t2;
	
	int64_t s = s2;
	int64_t t = t2;
	
	if (remainder != 0) {
        return GetBezoutIdentity(b, remainder, s2, s3, t2, t3);
    }
    return {s, t};
}

void DetermineConsecutiveRunOrder(std::vector<std::pair<int64_t, int64_t>> BusOffsets)
{
    // Actually I realised that the first number we look for is the one that the
    // last number divides without remainder. The remainders are inverted from the
    // offsets in the calculation.
    const auto& [LastBus, LastPrimeOffset] = BusOffsets.back();

    // "surely the actual earliest timestamp will be larger than 100000000000000!"
    // Don't think we need to worry about the above since we are not sieving anymore.
    int64_t Solution = 0;

    // Define and find the remainder for the first number for the first calculation.
    // In subsequent calculations it will be replaced by the answer to "x".
    int64_t FirstRemainder = LastPrimeOffset - BusOffsets[0].second;

    // Iterate through primes finding Bezout's Identity and generating the current
    // remainder for the next calculation.
    auto OffsetPointer = BusOffsets.begin();
    for (int i = 1; i < BusOffsets.size(); i++)
    {
        // The first number in the Bezout's identity needs to be the product
        // of all primes up until the second to current. Thus, the last calculation
        // is between the last prime and the product of all previous primes.
        // This is currently a massive mess due to std::accumulate not really working nicely
        // with the std::pair.
        int64_t FirstNumber = 1;
        for (auto it = BusOffsets.begin(); it <= OffsetPointer; it++)
        {
            FirstNumber *= (*it).first;
        }

        // generate bezout's identity for all primes so far up until the 
        const int64_t SecondNumber = BusOffsets[i].first;
        const auto& [s, t] = GetBezoutIdentity(FirstNumber, SecondNumber);

        // calculate the solution for this pair of numbers
        const int64_t SecondRemainder = LastPrimeOffset - BusOffsets[i].second;
        Solution = FirstRemainder * t * SecondNumber + SecondRemainder * s * FirstNumber;

        // Adjust the solution to be the smallest possible positive number.
        // This is a horrible way to do it but floor/ceil doesn't seem to be working nicely
        // with these types.
        int64_t Adjustment = (Solution < 0) ? FirstNumber * SecondNumber : -FirstNumber * SecondNumber;
        while (Solution < 0 || Solution > abs(Adjustment))
        {
            Solution += Adjustment;
        }

        // If there are further calculations, reuse the current round solution
        // as the remainder for the next round first number.
        FirstRemainder = Solution;

        // Move the offset to the next position in the vector.
        std::advance(OffsetPointer, 1);
    }

    // The "Solution" is actually for the last bus. We need to remove the
    // largest offset to get the current earliest timestamp.
    std::cout << Solution << std::endl;
    std::cout << LastPrimeOffset << std::endl;
    Solution -= LastPrimeOffset;
    std::cout << std::endl << "(Part2) Earliest timestamp all buses would leave in sequence is " << Solution << std::endl;
}

// Convert the list of buses like 23,x,x,5,7,x,9 into a sequence of bus numbers
// paired with their offset through the list (which is how long they are delayed
// from the start of the timetable).
std::vector<std::pair<int64_t, int64_t>> ParseTimetableOffsets(std::vector<std::string> Timetable)
{
    std::vector<std::pair<int64_t, int64_t>> BusToOffset;

    for (int i = 0; i < Timetable.size(); i++)
    {
        if (Timetable[i] == "x") continue;

        BusToOffset.push_back({std::stoll(Timetable[i]), (int64_t)i});
    }

    return BusToOffset;
}

int main(int argc, char** argv)
{
    int LineNum = 0;
    int64_t Timestamp = 0;
    std::string Timetable;

    // this would actually be ideal for a fiber or coroutine
    ReadInput([&LineNum, &Timestamp, &Timetable](std::string Line) {
        if (LineNum == 0)
        {
            LineNum++;
            Timestamp = std::stoll(Line);
            return;
        }
        Timetable = Line;
    });

    // Convert the Timetable into a list of all buses.
    // Part 1 will convert this into just the running buses.
    std::vector<std::string> Buses = ParseTimetable(Timetable);

    // Part 2 needs the list of Buses with their respective delays, signified by the gap
    // in x characters between buses.
    std::vector<std::pair<int64_t, int64_t>> BusOffsets = ParseTimetableOffsets(Buses);

    DetermineEarliestBus(Buses, Timestamp);

    DetermineConsecutiveRunOrder(BusOffsets);
}