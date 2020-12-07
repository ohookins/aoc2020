#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

const std::string INPUT_FILENAME = "input.txt";

// storage of all boarding passes
std::vector<std::string> BoardingPasses;

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

        BoardingPasses.push_back(std::move(Line));
    }
}

void decodePasses()
{
    int greatestPassNumber = 0;

    for (auto Pass : BoardingPasses)
    {
        // decode row
        int row = 0;
        for (int i = 0; i < 7; i++)
        {
            // F = 0, B = 1
            if (Pass[i] == 'F') continue;

            // we are going from MSB to LSB
            row += 1 << (6 - i);
        }

        // decode column
        int col = 0;
        for (int i = 7; i < 10; i++)
        {
            // L = 0, R = 1
            if (Pass[i] == 'L') continue;

            // again from MSB to LSB
            // although we have to adjust value as we started from part-way
            // through the string
            int colIndex = i - 7;
            col += 1 << (2 - colIndex);
        }

        int seatID = row * 8 + col;
        if (seatID > greatestPassNumber)
        {
            greatestPassNumber = seatID;
        }
    }

    std::cout << "Greatest pass number is " << greatestPassNumber << std::endl;
}

int main(int argc, char** argv)
{
    readInput();
    decodePasses();
}