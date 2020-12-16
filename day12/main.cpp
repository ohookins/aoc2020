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

std::pair<char, int> ParseLine(const std::string Line)
{
    std::regex DirectionRE("^(\\w)(\\d+)$");
    std::smatch Match;
    std::regex_match(Line, Match, DirectionRE);

    // we have a lot of confidence that this is a happy path!
    return {Match[1].str()[0], std::stoi(Match[2].str())};
}

// surely this is in the standard library cmath?
float DegreesToRadians(float Degrees)
{
    return Degrees * M_PI / 180.0f;
}

// Use trig to figure out where the ship should end up based on
// direction and distance
std::pair<float, float> CalculateDestination(float Direction, float Distance)
{
    float y = sin(DegreesToRadians(Direction)) * Distance;
    float x = cos(DegreesToRadians(Direction)) * Distance;
    return {x, y};
}

void Navigate(const std::vector<std::pair<char, int>> NavigationData)
{
    // Consider 0,0 our origin
    float CurrentX = 0.0f, CurrentY = 0.0f;

    // direction of the ship in degrees
    float Direction = 0.0f;

    // Iterate through instructions in the nav data
    for (const auto& [Instruction, Amount] : NavigationData)
    {
        std::cout << "Location: (" << CurrentX << "," << CurrentY << ") Direction: " << Direction << std::endl;
        switch(Instruction)
        {
            case 'N':
                CurrentY += Amount;
                break;
            case 'S':
                CurrentY -= Amount;
                break;
            case 'E':
                CurrentX += Amount;
                break;
            case 'W':
                CurrentX -= Amount;
                break;
            case 'L':
                Direction += Amount;
                if (Direction >= 360.0f) Direction -= 360.0f;
                break;
            case 'R':
                Direction -= Amount;
                if (Direction < 0.0f) Direction += 360.0f;
                break;
            case 'F':
                auto [X, Y] = CalculateDestination(Direction, Amount);
                CurrentX += X;
                CurrentY += Y;
                break;
        }
    }

    std::cout << "We ended up at (" << CurrentX << ", " << CurrentY << ")" << std::endl;
    std::cout << "Manhattan distance is: " << round(abs(CurrentX) + abs(CurrentY)) << std::endl;
}

int main(int argc, char** argv)
{
    std::vector<std::pair<char, int>> NavigationData;

    // Fill all cells except one line around the outside
    ReadInput([&NavigationData](std::string Line) {
        NavigationData.push_back(ParseLine(Line));
    });

    Navigate(NavigationData);
}