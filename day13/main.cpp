#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <regex>
#include <cmath>

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

std::vector<int> ParseTimetable(const std::string Timetable)
{
    std::vector<int> RunningBuses;

    // this is probably a bit risky but I'll assume we can rely on the comma separation
    std::regex BusRE("(\\d+)");
    std::smatch Match;
    std::string SearchString(Timetable);
    while (std::regex_search(SearchString, Match, BusRE))
    {
        SearchString = Match.suffix();

        if (Match[1].str() == "x") continue;

        RunningBuses.push_back(std::stoi(Match[1].str()));
    }

    return RunningBuses;
}

void DetermineEarliestBus(std::vector<int> Buses, int Timestamp)
{
    int EarliestTimestamp = INT_MAX;
    int EarliestBus = 0;

    for (int Bus : Buses)
    {
        // should give either the exact or rounded-down answer?
        int Quotient = Timestamp / Bus;
        
        // determine the earliest time close to the timestamp
        // that this bus leaves
        int EarliestTime = 0;
        if (Quotient * Bus == Timestamp) EarliestTime = Timestamp;
        else EarliestTime = (Quotient + 1) * Bus;

        // compare to the existing lowest number and replace if lower
        if (EarliestTime < EarliestTimestamp)
        {
            EarliestTimestamp = EarliestTime;
            EarliestBus = Bus;
        }
    }

    int Part1Answer = EarliestBus * (EarliestTimestamp - Timestamp);
    std::cout << "The earliest bus is " << EarliestBus << " at time " << EarliestTimestamp << std::endl;
    std::cout << "Would need to wait " << EarliestTimestamp - Timestamp << " minutes for it." << std::endl;
    std::cout << "Part1 Answer: " << Part1Answer << std::endl;
}

int main(int argc, char** argv)
{
    int LineNum = 0;
    int Timestamp = 0;
    std::string Timetable;

    // this would actually be ideal for a fiber or coroutine
    ReadInput([&LineNum, &Timestamp, &Timetable](std::string Line) {
        if (LineNum == 0)
        {
            LineNum++;
            Timestamp = std::stoi(Line);
            return;
        }
        Timetable = Line;
    });

    // convert the Timetable into a list of running buses
    std::vector<int> Buses = ParseTimetable(Timetable);

    DetermineEarliestBus(Buses, Timestamp);
}